# Лямбда-выражения в C++ и Unreal Engine

## Содержание
- [Что такое лямбда](#что-такое-лямбда)
- [Синтаксис](#синтаксис)
- [Список захвата (Capture List)](#список-захвата-capture-list)
- [Параметры и возвращаемое значение](#параметры-и-возвращаемое-значение)
- [Mutable лямбды](#mutable-лямбды)
- [Generic лямбды (auto)](#generic-лямбды-auto)
- [Лямбды и TArray](#лямбды-и-tarray)
- [Лямбды в Unreal Engine](#лямбды-в-unreal-engine)
- [Лямбда как параметр функции](#лямбда-как-параметр-функции)
- [Опасности и подводные камни](#опасности-и-подводные-камни)
- [Практические примеры для FPS-проекта](#практические-примеры-для-fps-проекта)
- [Шпаргалка](#шпаргалка)

---

## Что такое лямбда

Лямбда — это **анонимная функция**, которую можно объявить прямо в месте использования. Вместо того чтобы писать отдельную функцию или метод, ты создаёшь маленький блок кода "на лету".

**Аналогия:** обычная функция — это рецепт, записанный в кулинарной книге. Лямбда — это когда ты говоришь повару на месте: "возьми это, сделай вот так, верни результат". Без названия, без записи в книгу.

```cpp
// Обычная функция
int Double(int X) { return X * 2; }

// Лямбда — то же самое, но анонимно
auto Double = [](int X) { return X * 2; };
```

---

## Синтаксис

Полный синтаксис лямбды:

```
[Захват](Параметры) mutable -> ВозвращаемыйТип { Тело }
```

| Часть | Обязательна? | Описание |
|---|---|---|
| `[Захват]` | ✅ Да | Какие переменные из окружения доступны внутри |
| `(Параметры)` | ❌ Нет | Входные параметры (как у обычной функции) |
| `mutable` | ❌ Нет | Разрешает изменять захваченные по значению переменные |
| `-> Тип` | ❌ Нет | Явное указание типа возвращаемого значения |
| `{ Тело }` | ✅ Да | Код лямбды |

### Минимальная лямбда

```cpp
auto Hello = []() { UE_LOG(LogTemp, Log, TEXT("Hello!")); };
Hello(); // Вызов
```

Если параметров нет, скобки `()` можно опустить:

```cpp
auto Hello = [] { UE_LOG(LogTemp, Log, TEXT("Hello!")); };
```

---

## Список захвата (Capture List)

Самая важная часть лямбды. Определяет, какие **внешние переменные** лямбда "видит" и как.

### Захват по значению

Копирует переменную. Лямбда получает **копию** — оригинал не изменится.

```cpp
int Ammo = 30;

auto PrintAmmo = [Ammo]() {
    UE_LOG(LogTemp, Log, TEXT("Ammo: %d"), Ammo);
};

Ammo = 0;
PrintAmmo(); // Выведет 30 — захватили КОПИЮ
```

### Захват по ссылке

Лямбда работает с **оригинальной** переменной. Изменения внутри лямбды затронут оригинал.

```cpp
int Ammo = 30;

auto Reload = [&Ammo]() {
    Ammo = 30;
};

Ammo = 5;
Reload();
// Ammo теперь снова 30
```

### Все варианты захвата

```cpp
int A = 1, B = 2, C = 3;

[]()  {}               // Ничего не захватывать
[=]() {}               // Всё по значению (копии)
[&]() {}               // Всё по ссылке
[A]() {}               // Только A по значению
[&A]() {}              // Только A по ссылке
[A, &B]() {}           // A — копия, B — ссылка
[=, &B]() {}           // Всё копии, но B — ссылка
[&, A]() {}            // Всё ссылки, но A — копия
[this]() {}            // Указатель на текущий объект
[*this]() {}           // Копия всего объекта (C++17)
```

### Таблица захвата

| Синтаксис | Что делает |
|---|---|
| `[]` | Ничего не захватывает |
| `[=]` | Всё по значению |
| `[&]` | Всё по ссылке |
| `[x]` | Только `x` по значению |
| `[&x]` | Только `x` по ссылке |
| `[=, &x]` | Всё по значению, `x` по ссылке |
| `[&, x]` | Всё по ссылке, `x` по значению |
| `[this]` | Указатель на текущий объект |
| `[*this]` | Копия текущего объекта (C++17) |

---

## Параметры и возвращаемое значение

### С параметрами

```cpp
auto Add = [](int A, int B) { return A + B; };
int Result = Add(3, 4); // 7
```

### Явный тип возвращаемого значения

Обычно компилятор определяет тип сам. Но если логика сложная — укажи явно:

```cpp
auto Divide = [](float A, float B) -> float {
    if (B == 0.0f) return 0.0f;
    return A / B;
};
```

### Без параметров и без возвращаемого значения

```cpp
auto DoSomething = []() {
    UE_LOG(LogTemp, Warning, TEXT("Doing something!"));
};
DoSomething();
```

---

## Mutable лямбды

По умолчанию лямбда **не может изменять** переменные, захваченные по значению. `mutable` это разрешает:

```cpp
int Counter = 0;

// ❌ Ошибка компиляции — нельзя менять Counter, это копия
// auto Inc = [Counter]() { Counter++; };

// ✅ С mutable — можно менять КОПИЮ (оригинал НЕ изменится!)
auto Inc = [Counter]() mutable {
    Counter++;
    UE_LOG(LogTemp, Log, TEXT("Counter inside: %d"), Counter);
};

Inc(); // Counter inside: 1
Inc(); // Counter inside: 2
// Оригинальный Counter всё ещё 0!
```

**Важно:** `mutable` меняет **копию** внутри лямбды, а не оригинал. Если хочешь менять оригинал — используй захват по ссылке `[&Counter]`.

---

## Generic лямбды (auto)

С C++14 параметры могут быть `auto` — компилятор сам определит тип:

```cpp
auto Multiply = [](auto A, auto B) { return A * B; };

Multiply(3, 4);        // int * int = 12
Multiply(2.5f, 3.0f);  // float * float = 7.5
```

---

## Лямбды и TArray

### Sort — сортировка

```cpp
TArray<int> Numbers = {5, 2, 8, 1, 9};

// По возрастанию
Numbers.Sort([](int A, int B) {
    return A < B;
});
// Результат: 1, 2, 5, 8, 9

// По убыванию
Numbers.Sort([](int A, int B) {
    return A > B;
});
// Результат: 9, 8, 5, 2, 1
```

### Сортировка объектов по полю

```cpp
TArray<FWeaponInfo> Weapons;

Weapons.Sort([](const FWeaponInfo& A, const FWeaponInfo& B) {
    return A.Damage > B.Damage; // Сначала самое сильное
});
```

### FilterByPredicate — фильтрация

```cpp
TArray<int> Numbers = {1, 2, 3, 4, 5, 6, 7, 8};

TArray<int> EvenNumbers = Numbers.FilterByPredicate([](int N) {
    return N % 2 == 0;
});
// Результат: 2, 4, 6, 8
```

### FindByPredicate — поиск

```cpp
TArray<AActor*> Actors;

AActor** Found = Actors.FindByPredicate([](AActor* Actor) {
    return Actor->GetName().Contains(TEXT("Zombie"));
});

if (Found)
{
    // Нашли актора с "Zombie" в имени
}
```

### ContainsByPredicate — проверка наличия

```cpp
TArray<int> Numbers = {10, 20, 30};

bool bHasLarge = Numbers.ContainsByPredicate([](int N) {
    return N > 25;
});
// true
```

### RemoveAll — удаление по условию

```cpp
TArray<AActor*> Enemies;

int32 Removed = Enemies.RemoveAll([](AActor* Enemy) {
    return !IsValid(Enemy); // Удалить все невалидные
});
```

### IndexOfByPredicate — индекс элемента

```cpp
TArray<UFP_WeaponBase*> Weapons;

int32 Index = Weapons.IndexOfByPredicate([](UFP_WeaponBase* W) {
    return IsValid(W) && W->GetAmmo() == 0;
});
// Индекс первого оружия без патронов, или INDEX_NONE
```

---

## Лямбды в Unreal Engine

### Таймеры

Один из самых частых случаев:

```cpp
FTimerHandle TimerHandle;

// Повторяющийся таймер
GetWorldTimerManager().SetTimer(TimerHandle, [this]()
{
    Health -= 10.0f;
    UE_LOG(LogTemp, Log, TEXT("Health: %f"), Health);
}, 2.0f, true); // Каждые 2 секунды

// Одноразовая задержка
GetWorldTimerManager().SetTimer(TimerHandle, [this]()
{
    SpawnEnemy();
}, 3.0f, false); // Через 3 секунды, один раз

// Остановить
GetWorldTimerManager().ClearTimer(TimerHandle);
```

### Делегаты

```cpp
// BindLambda — для обычных делегатов
FTimerDelegate TimerDelegate;
TimerDelegate.BindLambda([this]()
{
    UE_LOG(LogTemp, Log, TEXT("Delegate Lambda!"));
});

// AddLambda — для мультикаст делегатов
OnWeaponFired.AddLambda([](int32 AmmoLeft)
{
    UE_LOG(LogTemp, Log, TEXT("Ammo: %d"), AmmoLeft);
});
```

> ⚠️ **`AddDynamic` НЕ работает с лямбдами!** Только `AddLambda`, `BindLambda` для обычных (не DYNAMIC) делегатов.

### Async / Потоки

```cpp
// Выполнить в Game Thread
AsyncTask(ENamedThreads::GameThread, [this]()
{
    UpdateUI();
});

// Выполнить в фоновом потоке
Async(EAsyncExecution::ThreadPool, [this]()
{
    CalculatePathfinding();
});
```

### ForEachComponent

```cpp
GetOwner()->ForEachComponent<UStaticMeshComponent>(false, [](UStaticMeshComponent* Comp)
{
    Comp->SetVisibility(false);
});
```

---

## Лямбда как параметр функции

### TFunctionRef — не владеет, только для вызова на стеке

```cpp
void ProcessEnemies(TArray<AActor*>& Enemies, TFunctionRef<void(AActor*)> Action)
{
    for (AActor* Enemy : Enemies)
    {
        if (IsValid(Enemy)) Action(Enemy);
    }
}

// Использование
ProcessEnemies(Enemies, [](AActor* Enemy) {
    Enemy->Destroy();
});
```

### TFunction — владеет, можно сохранить

```cpp
void ExecuteAfterDelay(float Delay, TFunction<void()> Callback)
{
    FTimerHandle Handle;
    GetWorldTimerManager().SetTimer(Handle, [Callback]()
    {
        Callback();
    }, Delay, false);
}

// Использование
ExecuteAfterDelay(2.0f, [this]() {
    SpawnWave();
});
```

### Шаблон — самый гибкий

```cpp
template<typename Predicate>
AActor* FindActorByCondition(const TArray<AActor*>& Actors, Predicate Pred)
{
    for (AActor* Actor : Actors)
    {
        if (Pred(Actor)) return Actor;
    }
    return nullptr;
}

// Использование
AActor* Boss = FindActorByCondition(Enemies, [](AActor* A) {
    return A->GetName().Contains(TEXT("Boss"));
});
```

### Сравнение типов

| Тип | Владеет | Копируется | Когда использовать |
|---|---|---|---|
| `TFunctionRef<void()>` | Нет | Нет | Параметр функции, лямбда живёт на стеке |
| `TFunction<void()>` | Да | Да | Сохранение лямбды, колбэки, таймеры |
| `std::function<void()>` | Да | Да | Вне Unreal кода |

> В Unreal предпочитай `TFunction` и `TFunctionRef` вместо `std::function`.

---

## Опасности и подводные камни

### 1. Dangling Reference — висячая ссылка

**Самая опасная ошибка.** Лямбда захватывает ссылку на локальную переменную, а та уже уничтожена когда лямбда вызывается.

```cpp
// ❌ КРАШ!
void AMyActor::StartTimer()
{
    int LocalVar = 42;

    GetWorldTimerManager().SetTimer(Handle, [&LocalVar]()
    {
        // LocalVar уже не существует!
        UE_LOG(LogTemp, Log, TEXT("%d"), LocalVar); // мусор или краш
    }, 5.0f, false);
}

// ✅ Безопасно — захват по значению
void AMyActor::StartTimer()
{
    int LocalVar = 42;

    GetWorldTimerManager().SetTimer(Handle, [LocalVar]()
    {
        UE_LOG(LogTemp, Log, TEXT("%d"), LocalVar); // OK — это копия
    }, 5.0f, false);
}
```

### 2. Захват this — объект может быть уничтожен

```cpp
// ❌ ОПАСНО!
void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
    {
        // this может быть уничтожен!
        Health = 100; // краш
    });
}

// ✅ Безопасно — TWeakObjectPtr
void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    TWeakObjectPtr<AMyActor> WeakThis(this);

    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [WeakThis]()
    {
        if (WeakThis.IsValid())
        {
            WeakThis->Health = 100;
        }
    });
}
```

### 3. Захват TSharedPtr для продления жизни

```cpp
TSharedPtr<FMyData> Data = MakeShared<FMyData>();

// Лямбда захватывает shared pointer — объект не умрёт, пока лямбда жива
auto Process = [Data]() {
    Data->DoWork();
};
```

### 4. Избегай [=] и [&] без необходимости

```cpp
// ❌ Плохо — захватывает ВСЁ, непонятно что используется
auto Bad = [=]() { return A + B; };

// ✅ Хорошо — явно видно что захвачено
auto Good = [A, B]() { return A + B; };
```

### 5. Не копируй огромные объекты

```cpp
TArray<FString> HugeArray; // 10000 элементов

// ❌ Плохо — копируется весь массив
auto Bad = [HugeArray]() { /* ... */ };

// ✅ Хорошо — ссылка (если массив переживёт лямбду)
auto Good = [&HugeArray]() { /* ... */ };
```

---

## Практические примеры для FPS-проекта

### Задержка перед респавном

```cpp
void AFP_GameMode::OnPlayerDeath(ACharacter* DeadPlayer)
{
    FTimerHandle RespawnHandle;
    TWeakObjectPtr<ACharacter> WeakPlayer(DeadPlayer);

    GetWorldTimerManager().SetTimer(RespawnHandle, [this, WeakPlayer]()
    {
        if (WeakPlayer.IsValid())
        {
            RespawnPlayer(WeakPlayer.Get());
        }
    }, 5.0f, false);
}
```

### Сортировка врагов по дистанции

```cpp
void AFP_Player::FindClosestEnemy()
{
    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieBase::StaticClass(), Enemies);

    FVector MyLocation = GetActorLocation();

    Enemies.Sort([MyLocation](AActor* A, AActor* B)
    {
        float DistA = FVector::DistSquared(A->GetActorLocation(), MyLocation);
        float DistB = FVector::DistSquared(B->GetActorLocation(), MyLocation);
        return DistA < DistB;
    });

    // Enemies[0] — ближайший
}
```

### Фильтр живых врагов

```cpp
TArray<AFP_BaseZombie*> AllZombies;

TArray<AFP_BaseZombie*> Alive = AllZombies.FilterByPredicate([](AFP_BaseZombie* Z) {
    return IsValid(Z) && Z->IsAlive();
});
```

### Поиск слота с определённым оружием

```cpp
int32 UFP_EquipmentManager::FindWeaponSlot(TSubclassOf<UFP_WeaponBase> WeaponClass)
{
    return EquippedWeapons.IndexOfByPredicate([WeaponClass](UFP_WeaponBase* W) {
        return IsValid(W) && W->IsA(WeaponClass);
    });
}
```

### Покупка оружия в магазине

```cpp
void UFP_ShopWidget::BindEvents()
{
    OnWeaponPurchased.AddLambda([this](const UFP_WeaponDataAsset* WeaponData)
    {
        if (EquipmentManager)
        {
            EquipmentManager->EquipNewWeapon(WeaponData);
        }
        RefreshUI();
    });
}
```

### Звук шагов с таймером

```cpp
void UFP_AudioComponent::StartFootsteps(bool bSprinting)
{
    float Interval = bSprinting ? 0.3f : 0.5f;

    GetWorld()->GetTimerManager().SetTimer(FootstepHandle, [this, bSprinting]()
    {
        const TArray<TObjectPtr<USoundBase>>& Sounds =
            bSprinting ? FootstepSettings.SprintSounds : FootstepSettings.WalkSounds;

        if (Sounds.Num() > 0)
        {
            int32 Index = FMath::RandRange(0, Sounds.Num() - 1);
            UGameplayStatics::PlaySoundAtLocation(this, Sounds[Index], GetOwner()->GetActorLocation());
        }
    }, Interval, true);
}
```

---

## Шпаргалка

```
┌──────────────────────────────────────────────────┐
│              АНАТОМИЯ ЛЯМБДЫ                      │
│                                                   │
│   [захват](параметры) -> тип { тело }            │
│    │         │            │      │                │
│    │         │            │      └─ Код            │
│    │         │            └─ Возвращаемый тип      │
│    │         └─ Входные аргументы                  │
│    └─ Внешние переменные                          │
└──────────────────────────────────────────────────┘

ЗАХВАТ:
  []        — ничего
  [=]       — всё по значению
  [&]       — всё по ссылке
  [x]       — x по значению
  [&x]      — x по ссылке
  [=, &x]   — всё по значению, x по ссылке
  [&, x]    — всё по ссылке, x по значению
  [this]    — указатель на объект
  [*this]   — копия объекта (C++17)

ЧАСТЫЕ ПАТТЕРНЫ В UE:

  // Таймер
  GetWorldTimerManager().SetTimer(H, [this](){ ... }, Time, bLoop);

  // Делегат
  MyDelegate.BindLambda([](){ ... });
  MyMulticast.AddLambda([](){ ... });

  // Сортировка
  Array.Sort([](const T& A, const T& B){ return A < B; });

  // Фильтр
  Array.FilterByPredicate([](const T& X){ return условие; });

  // Поиск
  Array.FindByPredicate([](const T& X){ return условие; });

  // Удаление
  Array.RemoveAll([](const T& X){ return условие; });

  // Индекс
  Array.IndexOfByPredicate([](const T& X){ return условие; });

  // Проверка
  Array.ContainsByPredicate([](const T& X){ return условие; });

  // Async
  AsyncTask(ENamedThreads::GameThread, [this](){ ... });

ПРАВИЛА БЕЗОПАСНОСТИ:
  ✅ Захватывай явно — [A, &B] вместо [=] или [&]
  ✅ TWeakObjectPtr для UObject в async/таймерах
  ✅ По значению для отложенных вызовов
  ❌ Не захватывай локальные переменные по ссылке в async
  ❌ Не используй [this] в async без проверки валидности
  ❌ AddDynamic НЕ работает с лямбдами
```

