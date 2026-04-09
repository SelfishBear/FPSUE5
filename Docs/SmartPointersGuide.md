# Умные указатели в Unreal Engine — Полный гайд

## Проблема: почему обычные указатели опасны

```cpp
// Обычный (сырой) указатель
AActor* MyActor = GetWorld()->SpawnActor<AActor>(...);
MyActor->Destroy(); // Актор удалён движком
MyActor->DoSomething(); // 💀 КРАШ — указатель висит в воздухе (dangling pointer)
```

Сырой указатель — это просто **адрес в памяти**. Он не знает, жив объект или уже удалён. Это как записать адрес дома на бумажке — дом могли снести, а бумажка осталась.

---

## Аналогия: Библиотека и книги

Представь библиотеку:

| Указатель | Аналогия |
|---|---|
| **Сырой указатель** `T*` | Записка с номером полки. Книгу могли убрать, а записка осталась |
| **`TSharedPtr`** | Читательский билет. Пока хоть один билет выдан — книга на полке. Последний билет сдали — книгу убирают |
| **`TWeakPtr`** | Ты помнишь, что книга была. Можешь прийти и спросить «она ещё есть?», но книга не будет ждать тебя |
| **`TUniquePtr`** | Книга выдана тебе лично. Никто другой не может её взять. Вернул — книгу убрали |
| **`TSharedRef`** | Как `TSharedPtr`, но гарантия что книга **точно** есть (не может быть null) |
| **`UPROPERTY() T*`** | Библиотекарь (GC) сам следит за книгой. Пока ты записан — не уберёт |
| **`TWeakObjectPtr`** | Как `TWeakPtr`, но для UObject (работает с GC движка) |
| **`TSoftObjectPtr`** | Закладка в каталоге. Книга может быть даже не загружена с полки, но ты знаешь где она |

---

## 1. `TSharedPtr<T>` — Общее владение

### Как работает

`TSharedPtr` использует **подсчёт ссылок** (reference counting). Внутри хранится:
- Указатель на объект
- Указатель на **счётчик ссылок** (общий для всех копий)

```
TSharedPtr<FMyClass> A ──┐
                         ├──► [Счётчик: 2] ──► [FMyClass объект]
TSharedPtr<FMyClass> B ──┘
```

Когда создаёшь копию — счётчик увеличивается. Когда `TSharedPtr` уничтожается — уменьшается. Когда счётчик **дошёл до 0** — объект удаляется.

### Создание

```cpp
// Правильный способ (один аллокация: счётчик + объект вместе)
TSharedPtr<FMyClass> Ptr = MakeShared<FMyClass>(Arg1, Arg2);

// Альтернативный способ (две аллокации)
TSharedPtr<FMyClass> Ptr = MakeShareable(new FMyClass(Arg1, Arg2));
```

> ⚡ **`MakeShared` предпочтительнее** — он выделяет память один раз (и для объекта, и для счётчика), что быстрее и эффективнее.

### Пример

```cpp
void Example()
{
    TSharedPtr<FWeaponConfig> Config = MakeShared<FWeaponConfig>();
    Config->Damage = 50.f;

    // Копируем — счётчик стал 2
    TSharedPtr<FWeaponConfig> AnotherRef = Config;

    // Config выходит из области видимости — счётчик стал 1
    // AnotherRef выходит из области видимости — счётчик стал 0 — объект удалён
}
```

### Проверка валидности

```cpp
if (Config.IsValid())  // или if (Config)
{
    Config->Damage = 100.f;
}
```

### Сброс

```cpp
Config.Reset(); // Уменьшает счётчик, указатель становится null
Config = nullptr; // То же самое
```

### Когда использовать

- Несколько систем владеют одним объектом
- Объект должен жить, пока кто-то им пользуется
- **Только для НЕ-UObject классов** (обычные C++ классы, структуры)

---

## 2. `TSharedRef<T>` — Гарантированная ссылка

### Отличие от TSharedPtr

`TSharedRef` — это `TSharedPtr`, который **никогда не может быть null**.

```cpp
// TSharedPtr — МОЖЕТ быть null
TSharedPtr<FMyClass> MaybeNull;  // OK, пустой

// TSharedRef — НЕ МОЖЕТ быть null
TSharedRef<FMyClass> AlwaysValid = MakeShared<FMyClass>(); // Обязательно инициализировать
```

### Пример

```cpp
TSharedRef<FWeaponConfig> CreateDefaultConfig()
{
    // Вызывающий код ГАРАНТИРОВАННО получает валидный объект
    return MakeShared<FWeaponConfig>();
}

void UseConfig(TSharedRef<FWeaponConfig> Config)
{
    // Не нужно проверять на null — он точно валиден
    Config->Damage = 100.f;
}
```

### Конвертация

```cpp
// SharedRef → SharedPtr (всегда безопасно)
TSharedPtr<FMyClass> Ptr = MyRef;

// SharedPtr → SharedRef (опасно, крашнет если null)
TSharedRef<FMyClass> Ref = Ptr.ToSharedRef();
```

### Когда использовать

- В параметрах функций, когда null недопустим
- Как возвращаемое значение, чтобы гарантировать результат
- Когда хочешь избавиться от проверок на null

---

## 3. `TWeakPtr<T>` — Наблюдатель без владения

### Как работает

`TWeakPtr` **не увеличивает счётчик ссылок**. Он только «наблюдает». Объект может быть удалён в любой момент, и `TWeakPtr` об этом узнает.

```
TSharedPtr<FMyClass> Owner ──► [Счётчик: 1] ──► [FMyClass объект]
                                     ▲
TWeakPtr<FMyClass> Observer ─────────┘  (не увеличивает счётчик)
```

Если `Owner` уничтожен — счётчик = 0 — объект удалён. `Observer` теперь знает, что объект мёртв.

### Зачем нужен

**Решает проблему циклических ссылок:**

```
// ПРОБЛЕМА: утечка памяти
A (TSharedPtr) → B
B (TSharedPtr) → A
// Счётчик никогда не дойдёт до 0!

// РЕШЕНИЕ:
A (TSharedPtr) → B
B (TWeakPtr)   → A    // Не держит A в живых
```

### Пример

```cpp
class FQuestSystem
{
    TSharedPtr<FQuestData> ActiveQuest;
};

class FUIWidget
{
    // UI не владеет квестом — только отображает
    TWeakPtr<FQuestData> DisplayedQuest;

    void UpdateUI()
    {
        // Pin() — попытка получить TSharedPtr
        // Если объект жив — вернёт валидный TSharedPtr (и счётчик временно +1)
        // Если мёртв — вернёт null
        TSharedPtr<FQuestData> Quest = DisplayedQuest.Pin();
        if (Quest.IsValid())
        {
            // Безопасно работаем
            UpdateQuestText(Quest->Description);
        }
        else
        {
            // Квест удалён, очищаем UI
            ClearQuestUI();
        }
    }
};
```

### Когда использовать

- Кэширование / наблюдение за объектом без владения
- Разрыв циклических зависимостей
- UI, который отображает данные, но не владеет ими

---

## 4. `TUniquePtr<T>` — Единоличное владение

### Как работает

Только **один** `TUniquePtr` может владеть объектом. Нельзя копировать, можно только **перемещать** (`MoveTemp`).

```
TUniquePtr<FMyClass> Owner ──► [FMyClass объект]
// Нет счётчика ссылок — максимальная производительность
```

### Пример

```cpp
// Создание
TUniquePtr<FWeaponState> State = MakeUnique<FWeaponState>();
State->AmmoCount = 30;

// ОШИБКА: нельзя копировать
// TUniquePtr<FWeaponState> Copy = State;  // ❌ Не компилируется

// Перемещение — передаём владение
TUniquePtr<FWeaponState> NewOwner = MoveTemp(State);
// Теперь State == nullptr, NewOwner владеет объектом
```

### В классах

```cpp
class FWeaponManager
{
    // Менеджер — единственный владелец конфига
    TUniquePtr<FBallisticsCalculator> Calculator;

public:
    FWeaponManager()
        : Calculator(MakeUnique<FBallisticsCalculator>())
    {}

    // Передаём только сырой указатель для чтения (без владения)
    FBallisticsCalculator* GetCalculator() const { return Calculator.Get(); }
};
```

### Когда использовать

- Объект принадлежит строго одному владельцу
- Не нужно делиться владением
- Максимальная производительность (нет счётчика)
- Pimpl-паттерн (скрытие реализации)

---

## 5. UObject-мир: `UPROPERTY()`, `TWeakObjectPtr`, `TSoftObjectPtr`

### ⚠️ Главное правило

> **Для UObject (AActor, UActorComponent, и т.д.) НЕЛЬЗЯ использовать `TSharedPtr` / `TUniquePtr`!**
> UObject управляются **Garbage Collector'ом** (GC) Unreal Engine.

### `UPROPERTY() T*` — Стандартный способ

```cpp
UCLASS()
class UFP_EquipmentManager : public UObject
{
    GENERATED_BODY()

    // GC видит эту ссылку и НЕ удалит WeaponActor, пока она жива
    UPROPERTY()
    AFPWeaponActor* WeaponActor;

    // ❌ ОПАСНО: GC не видит этот указатель — может удалить объект
    AFPWeaponActor* UnsafePtr;
};
```

**Как работает GC:**
1. GC периодически сканирует все UObject
2. Ищет, кто на кого ссылается через `UPROPERTY()`
3. Если на объект **никто** не ссылается — удаляет его
4. Если указатель **не** помечен `UPROPERTY()` — GC его **не видит**

**Аналогия:** GC — библиотекарь. `UPROPERTY()` — запись в формуляре. Если нигде не записано что книга выдана — библиотекарь её спишет.

### `TWeakObjectPtr<T>` — Слабая ссылка на UObject

```cpp
// Не мешает GC удалить объект
TWeakObjectPtr<AActor> WeakEnemy;

void CheckEnemy()
{
    if (WeakEnemy.IsValid())
    {
        // Враг ещё жив
        float Dist = FVector::Dist(GetActorLocation(), WeakEnemy->GetActorLocation());
    }
    else
    {
        // Враг удалён / уничтожен
        WeakEnemy.Reset();
    }
}
```

**Когда использовать:**
- Ссылка на актора, который может быть уничтожен в любой момент
- Не хочешь мешать GC
- Кэш / наблюдение

### `TSoftObjectPtr<T>` — Ленивая загрузка ассетов

```cpp
UPROPERTY(EditAnywhere)
TSoftObjectPtr<UStaticMesh> WeaponMesh;

void LoadWeapon()
{
    // Проверяем, загружен ли ассет
    if (WeaponMesh.IsValid())
    {
        // Уже в памяти — используем
        UStaticMesh* Mesh = WeaponMesh.Get();
    }
    else
    {
        // Загружаем синхронно
        UStaticMesh* Mesh = WeaponMesh.LoadSynchronous();
    }
}
```

**Аналогия:** Ты знаешь название книги и полку, но книга может быть убрана в архив. `LoadSynchronous()` — попросить принести её из архива.

**Когда использовать:**
- Не хочешь загружать ассет в память сразу (экономия RAM)
- Ассет может не понадобиться (например, скин которого нет у игрока)

### `TSoftClassPtr<T>` — Ленивая загрузка классов

```cpp
UPROPERTY(EditAnywhere)
TSoftClassPtr<AWeaponBase> WeaponClass;

void SpawnWeapon()
{
    UClass* LoadedClass = WeaponClass.LoadSynchronous();
    if (LoadedClass)
    {
        GetWorld()->SpawnActor<AWeaponBase>(LoadedClass);
    }
}
```

---

## 6. `TObjectPtr<T>` — Замена сырым указателям в UE5

Начиная с UE5, для **UPROPERTY** указателей на UObject рекомендуется:

```cpp
// UE4 стиль
UPROPERTY()
UStaticMeshComponent* MeshComp;

// UE5 стиль (рекомендуется)
UPROPERTY()
TObjectPtr<UStaticMeshComponent> MeshComp;
```

**В чём разница:** `TObjectPtr` добавляет опциональную проверку доступа и ленивую загрузку в редакторе. В рантайме (shipping build) работает как обычный указатель. Использование **не обязательно**, но это направление UE5.

---

## Полная сводная таблица

| Тип | Владение | Null? | Для UObject? | Счётчик | Копирование |
|---|---|---|---|---|---|
| `T*` (сырой) | Нет | Да | Только с UPROPERTY | Нет | Да |
| `TSharedPtr<T>` | Общее | Да | ❌ Нет | Да | Да (+1) |
| `TSharedRef<T>` | Общее | ❌ Нет | ❌ Нет | Да | Да (+1) |
| `TWeakPtr<T>` | Нет | Да | ❌ Нет | Не влияет | Да |
| `TUniquePtr<T>` | Единоличное | Да | ❌ Нет | Нет | ❌ MoveTemp |
| `UPROPERTY() T*` | GC | Да | ✅ Да | GC | Да |
| `TWeakObjectPtr<T>` | Нет | Да | ✅ Да | GC | Да |
| `TSoftObjectPtr<T>` | Нет/Ленивая | Да | ✅ Да | GC | Да |
| `TObjectPtr<T>` | GC | Да | ✅ Да | GC | Да |

---

## Дерево решений: что выбрать?

```
Объект — это UObject (AActor, UActorComponent, UObject)?
├── ДА
│   ├── Нужно хранить ссылку, объект должен жить? → UPROPERTY() T* или TObjectPtr<T>
│   ├── Объект может быть уничтожен, нужна безопасная проверка? → TWeakObjectPtr<T>
│   ├── Ассет, который не нужно грузить сразу? → TSoftObjectPtr<T>
│   └── Класс для будущего спавна? → TSoftClassPtr<T> или TSubclassOf<T>
│
└── НЕТ (обычный C++ класс / структура)
    ├── Один владелец? → TUniquePtr<T>
    ├── Несколько владельцев? → TSharedPtr<T>
    ├── Нужна гарантия не-null? → TSharedRef<T>
    └── Наблюдение без владения? → TWeakPtr<T>
```

---

## Частые ошибки

### ❌ TSharedPtr для UObject

```cpp
// НЕПРАВИЛЬНО — GC не знает про TSharedPtr, будет double-free
TSharedPtr<AActor> Bad = MakeShareable(SomeActor);

// ПРАВИЛЬНО
UPROPERTY()
AActor* Good;
```

### ❌ Забыл UPROPERTY()

```cpp
// ОПАСНО — GC не видит ссылку и может удалить объект
AActor* MyActor = SpawnActor<AActor>(...);

// БЕЗОПАСНО
UPROPERTY()
AActor* MyActor;
```

### ❌ Циклические TSharedPtr

```cpp
// УТЕЧКА ПАМЯТИ
struct FA { TSharedPtr<FB> B; };
struct FB { TSharedPtr<FA> A; };

// РЕШЕНИЕ — одна из ссылок должна быть Weak
struct FA { TSharedPtr<FB> B; };
struct FB { TWeakPtr<FA> A; };  // ✅
```

### ❌ Использование TWeakPtr без Pin()

```cpp
TWeakPtr<FMyClass> Weak = ...;

// НЕПРАВИЛЬНО — нет метода ->
// Weak->DoSomething(); // ❌ Не компилируется

// ПРАВИЛЬНО
if (TSharedPtr<FMyClass> Pinned = Weak.Pin())
{
    Pinned->DoSomething(); // ✅
}
```

### ❌ Хранение TSharedPtr из this

```cpp
class FMyClass : public TSharedFromThis<FMyClass>
{
    void RegisterSelf()
    {
        // НЕПРАВИЛЬНО — создаст второй счётчик
        // TSharedPtr<FMyClass> Self(this);

        // ПРАВИЛЬНО — использует существующий счётчик
        TSharedPtr<FMyClass> Self = AsShared();
    }
};
```

---

## TSharedFromThis — получить TSharedPtr на самого себя

Если объекту нужно передать `TSharedPtr` на себя куда-то:

```cpp
class FQuestManager : public TSharedFromThis<FQuestManager>
{
    void StartQuest()
    {
        // Получаем TSharedPtr на this (безопасно)
        TSharedPtr<FQuestManager> Self = AsShared();
        
        QuestUI->SetManager(Self);
    }
};

// Объект ОБЯЗАТЕЛЬНО должен уже быть в TSharedPtr:
TSharedPtr<FQuestManager> Manager = MakeShared<FQuestManager>();
Manager->StartQuest(); // ✅ Работает

// FQuestManager StackObj;
// StackObj.StartQuest(); // ❌ КРАШ — объект не в TSharedPtr
```

---

## Потокобезопасность

По умолчанию `TSharedPtr` **не потокобезопасен**. Для многопоточности:

```cpp
// Потокобезопасная версия (атомарный счётчик)
TSharedPtr<FMyClass, ESPMode::ThreadSafe> SafePtr = MakeShared<FMyClass, ESPMode::ThreadSafe>();

// Обычная версия (быстрее, но только один поток)
TSharedPtr<FMyClass, ESPMode::NotThreadSafe> FastPtr = MakeShared<FMyClass>();
// ESPMode::NotThreadSafe — режим по умолчанию
```

---

## Практический пример: Оружейная система

```cpp
// Конфиг оружия — обычный C++ класс, общее владение
class FWeaponConfig
{
public:
    float Damage;
    float FireRate;
    int32 MagSize;
};

// Менеджер оружия — UObject (управляется GC)
UCLASS()
class UFP_WeaponManager : public UObject
{
    GENERATED_BODY()

    // UObject ссылки — через UPROPERTY
    UPROPERTY()
    TArray<AFPWeaponActor*> WeaponActors;

    // Обычные C++ объекты — через умные указатели
    TSharedPtr<FWeaponConfig> SharedConfig;   // Конфиг, который шарят несколько систем
    TUniquePtr<FBallisticsEngine> Ballistics; // Движок баллистики — один владелец

    // Безопасная ссылка на текущую цель
    TWeakObjectPtr<AActor> CurrentTarget;

    // Ассет, который загрузим когда понадобится
    UPROPERTY(EditAnywhere)
    TSoftObjectPtr<USoundBase> FireSound;
};
```

---

## Итого

1. **UObject** → `UPROPERTY() T*`, `TWeakObjectPtr`, `TSoftObjectPtr`
2. **Обычный C++** → `TUniquePtr` (один владелец), `TSharedPtr` (много), `TWeakPtr` (наблюдение)
3. **Никогда** не мешай `TSharedPtr` и UObject/GC
4. **`MakeShared`** > `MakeShareable` (производительность)
5. **`TSharedRef`** когда null недопустим
6. **`TWeakPtr`** для разрыва циклов и кэширования

