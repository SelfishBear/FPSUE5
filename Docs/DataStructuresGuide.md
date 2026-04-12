# Структуры данных в C++ и Unreal Engine — Полный гайд

## Содержание
- [Стандартные структуры C++](#стандартные-структуры-c)
  - [std::array](#stdarray)
  - [std::vector](#stdvector)
  - [std::list](#stdlist)
  - [std::map и std::unordered_map](#stdmap-и-stdunordered_map)
  - [std::set и std::unordered_set](#stdset-и-stdunordered_set)
  - [std::stack, std::queue, std::deque](#stdstack-stdqueue-stddeque)
  - [std::pair и std::tuple](#stdpair-и-stdtuple)
- [Контейнеры Unreal Engine](#контейнеры-unreal-engine)
  - [TArray](#tarray)
  - [TMap](#tmap)
  - [TSet](#tset)
  - [TQueue](#tqueue)
  - [TLinkedList](#tlinkedlist)
  - [TDoubleLinkedList](#tdoublelinkedlist)
  - [TPair и TTuple](#tpair-и-ttuple)
  - [TOptional](#toptional)
  - [TVariant](#tvariant)
  - [TBitArray](#tbitarray)
  - [TSparseArray](#tsparsearray)
  - [TStaticArray](#tstaticarray)
- [USTRUCT — пользовательские структуры](#ustruct--пользовательские-структуры)
  - [Объявление USTRUCT](#объявление-ustruct)
  - [USTRUCT + UPROPERTY](#ustruct--uproperty)
  - [Методы внутри структур](#методы-внутри-структур)
  - [Конструкторы](#конструкторы)
  - [Наследование структур](#наследование-структур)
  - [Структуры в блюпринтах](#структуры-в-блюпринтах)
  - [Структуры как параметры функций](#структуры-как-параметры-функций)
  - [Data Table Row](#data-table-row)
- [Сравнение: что когда использовать](#сравнение-что-когда-использовать)
- [Практические примеры](#практические-примеры)
- [Шпаргалка](#шпаргалка)

---

## Стандартные структуры C++

> ⚠️ В Unreal Engine **предпочитай контейнеры UE** (`TArray`, `TMap`, `TSet`) вместо стандартных. Они интегрированы с GC, сериализацией, UPROPERTY и блюпринтами. Стандартные контейнеры описаны для общего понимания.

---

### std::array

Массив **фиксированного** размера. Размер задаётся на этапе компиляции.

```cpp
#include <array>

std::array<int, 5> Numbers = {10, 20, 30, 40, 50};

Numbers[0];           // 10
Numbers.at(2);        // 30 (с проверкой границ)
Numbers.size();       // 5
Numbers.front();      // 10
Numbers.back();       // 50
Numbers.fill(0);      // Заполнить все нулями
```

**Когда:** фиксированное количество элементов, известное на этапе компиляции.

---

### std::vector

Динамический массив. Аналог `TArray` в UE.

```cpp
#include <vector>

std::vector<int> Numbers;

Numbers.push_back(10);       // Добавить в конец
Numbers.push_back(20);
Numbers.emplace_back(30);    // Создать прямо в массиве (без копирования)
Numbers.size();               // 3
Numbers[0];                   // 10
Numbers.front();              // 10
Numbers.back();               // 30
Numbers.pop_back();           // Удалить последний
Numbers.insert(Numbers.begin() + 1, 15); // Вставить 15 на позицию 1
Numbers.erase(Numbers.begin());          // Удалить первый
Numbers.clear();              // Очистить всё
Numbers.empty();              // true
Numbers.reserve(100);         // Зарезервировать память под 100 элементов
Numbers.shrink_to_fit();      // Освободить неиспользуемую память
```

**Когда:** динамический список, частый доступ по индексу.

---

### std::list

Двусвязный список. Быстрая вставка/удаление в любом месте, но нет доступа по индексу.

```cpp
#include <list>

std::list<int> Numbers;

Numbers.push_back(10);        // В конец
Numbers.push_front(5);        // В начало
Numbers.pop_back();           // Удалить последний
Numbers.pop_front();          // Удалить первый
Numbers.size();
Numbers.empty();

// Итерация
for (int N : Numbers) { /* ... */ }

// Вставка перед позицией
auto It = Numbers.begin();
std::advance(It, 2);          // Передвинуть итератор на 2
Numbers.insert(It, 15);

// Удаление по значению
Numbers.remove(15);           // Удалить все элементы со значением 15
```

**Когда:** частая вставка/удаление в середине, не нужен доступ по индексу.

---

### std::map и std::unordered_map

Словарь — пары ключ-значение.

| Тип | Порядок | Структура | Поиск |
|-----|---------|-----------|-------|
| `std::map` | Отсортирован по ключу | Красно-чёрное дерево | O(log n) |
| `std::unordered_map` | Без порядка | Хеш-таблица | O(1) в среднем |

```cpp
#include <map>
#include <unordered_map>

std::map<FString, int> Inventory;
// или
std::unordered_map<std::string, int> Inventory;

Inventory["Rifle"] = 1;
Inventory["Pistol"] = 2;
Inventory.insert({"Knife", 1});

Inventory["Rifle"];                  // 1
Inventory.count("Knife");            // 1 (есть) или 0 (нет)
Inventory.find("Pistol");            // Итератор
Inventory.erase("Pistol");           // Удалить по ключу
Inventory.size();
Inventory.clear();

// Итерация
for (auto& [Key, Value] : Inventory)
{
    // Key = "Rifle", Value = 1
}

// Проверка наличия ключа
if (Inventory.contains("Rifle"))  // C++20
{
    // ...
}
```

**Когда:** нужно связать ключ и значение, быстрый поиск по ключу.

---

### std::set и std::unordered_set

Множество — коллекция **уникальных** элементов.

| Тип | Порядок | Поиск |
|-----|---------|-------|
| `std::set` | Отсортирован | O(log n) |
| `std::unordered_set` | Без порядка | O(1) |

```cpp
#include <set>

std::set<int> UniqueNumbers;

UniqueNumbers.insert(10);
UniqueNumbers.insert(20);
UniqueNumbers.insert(10);     // Игнорируется — уже есть
UniqueNumbers.size();          // 2
UniqueNumbers.count(10);      // 1
UniqueNumbers.erase(20);
UniqueNumbers.clear();
```

**Когда:** нужно хранить уникальные значения, быстро проверять наличие.

---

### std::stack, std::queue, std::deque

#### std::stack — LIFO (последний вошёл, первый вышел)

```cpp
#include <stack>

std::stack<int> Stack;
Stack.push(10);
Stack.push(20);
Stack.push(30);
Stack.top();       // 30 (посмотреть верхний)
Stack.pop();       // Удалить верхний (30)
Stack.top();       // 20
Stack.size();      // 2
Stack.empty();
```

**Аналогия:** стопка тарелок. Берёшь верхнюю.

#### std::queue — FIFO (первый вошёл, первый вышел)

```cpp
#include <queue>

std::queue<int> Queue;
Queue.push(10);
Queue.push(20);
Queue.push(30);
Queue.front();     // 10 (первый)
Queue.back();      // 30 (последний)
Queue.pop();       // Удалить первый (10)
Queue.front();     // 20
```

**Аналогия:** очередь в магазине. Кто первый встал, тот первый обслужен.

#### std::deque — двусторонняя очередь

```cpp
#include <deque>

std::deque<int> Deque;
Deque.push_back(10);
Deque.push_front(5);
Deque.pop_back();
Deque.pop_front();
Deque[0];           // Доступ по индексу (в отличие от list)
```

---

### std::pair и std::tuple

#### std::pair — пара из двух значений

```cpp
#include <utility>

std::pair<FString, int> WeaponAmmo = {"Rifle", 30};

WeaponAmmo.first;    // "Rifle"
WeaponAmmo.second;   // 30

auto P = std::make_pair("Pistol", 12);
```

#### std::tuple — кортеж из N значений

```cpp
#include <tuple>

std::tuple<FString, int, float> WeaponInfo = {"Rifle", 30, 45.5f};

std::get<0>(WeaponInfo);   // "Rifle"
std::get<1>(WeaponInfo);   // 30
std::get<2>(WeaponInfo);   // 45.5f

// Structured binding (C++17)
auto [Name, Ammo, Damage] = WeaponInfo;
```

---

## Контейнеры Unreal Engine

> ✅ В Unreal **всегда предпочитай** эти контейнеры — они работают с GC, UPROPERTY, сериализацией и блюпринтами.

---

### TArray

**Главный контейнер UE.** Динамический массив, аналог `std::vector`.

#### Создание

```cpp
TArray<int32> Numbers;
TArray<int32> Numbers = {1, 2, 3, 4, 5};
TArray<FString> Names = {TEXT("Rifle"), TEXT("Pistol")};
TArray<AActor*> Actors;
```

#### Добавление

```cpp
Numbers.Add(10);              // Добавить в конец (копия)
Numbers.AddUnique(10);        // Добавить только если нет такого
Numbers.Emplace(20);          // Создать прямо в массиве (без копирования)
Numbers.Insert(15, 1);        // Вставить 15 на позицию 1
Numbers.Append({30, 40, 50}); // Добавить несколько
Numbers.SetNum(10);           // Установить размер (заполнит дефолтами)
Numbers.Init(0, 10);          // 10 элементов, все = 0
```

#### Доступ

```cpp
Numbers[0];                    // По индексу (без проверки!)
Numbers.Last();                // Последний элемент
Numbers.Last(1);               // Предпоследний
Numbers.Top();                 // То же что Last()
Numbers[Numbers.Num() - 1];   // Последний вручную
Numbers.IsValidIndex(5);      // Проверка индекса
```

#### Размер

```cpp
Numbers.Num();                 // Количество элементов
Numbers.Max();                 // Размер выделенной памяти
Numbers.IsEmpty();             // Пустой ли
Numbers.Reserve(100);          // Зарезервировать память
Numbers.Shrink();              // Освободить лишнюю память
```

#### Удаление

```cpp
Numbers.RemoveAt(0);                    // По индексу (сохраняет порядок)
Numbers.RemoveAtSwap(0);                // По индексу (быстрее, меняет порядок)
Numbers.Remove(10);                     // Первый элемент со значением 10
Numbers.RemoveSingle(10);               // Только первый найденный
Numbers.RemoveAll([](int N){ return N < 0; }); // По условию
Numbers.Pop();                           // Удалить и вернуть последний
Numbers.Empty();                         // Очистить всё (освобождает память)
Numbers.Reset();                         // Очистить без освобождения памяти
```

#### Поиск

```cpp
Numbers.Contains(10);                    // Есть ли значение
Numbers.Find(10);                        // Индекс первого вхождения (INDEX_NONE если нет)
Numbers.FindLast(10);                    // Индекс последнего вхождения

// По условию (лямбда)
int32* Found = Numbers.FindByPredicate([](int N) { return N > 50; });
int32 Index = Numbers.IndexOfByPredicate([](int N) { return N > 50; });
bool bHas = Numbers.ContainsByPredicate([](int N) { return N > 50; });
```

#### Сортировка

```cpp
// По умолчанию (по возрастанию)
Numbers.Sort();

// Своё условие
Numbers.Sort([](int A, int B) { return A > B; }); // По убыванию

// Стабильная сортировка (сохраняет порядок равных)
Numbers.StableSort([](int A, int B) { return A < B; });

// Сортировка объектов по полю
Weapons.Sort([](const FWeaponInfo& A, const FWeaponInfo& B) {
    return A.Damage > B.Damage;
});
```

#### Фильтрация

```cpp
TArray<int32> Positive = Numbers.FilterByPredicate([](int N) {
    return N > 0;
});
```

#### Итерация

```cpp
// Range-based for
for (int32 N : Numbers) { /* ... */ }

// По ссылке (без копирования)
for (int32& N : Numbers) { N *= 2; }

// По константной ссылке
for (const int32& N : Numbers) { /* ... */ }

// С индексом
for (int32 i = 0; i < Numbers.Num(); i++)
{
    Numbers[i];
}

// Обратная итерация
for (int32 i = Numbers.Num() - 1; i >= 0; i--)
{
    Numbers[i];
}
```

#### UPROPERTY

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
TArray<FWeaponInfo> WeaponSlots;

UPROPERTY(EditAnywhere, BlueprintReadWrite)
TArray<TSubclassOf<AActor>> EnemyClasses;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
TArray<AActor*> SpawnedEnemies;
```

---

### TMap

Словарь — пары ключ-значение. Аналог `std::unordered_map`. Ключи **уникальны**.

#### Создание

```cpp
TMap<FString, int32> Inventory;
TMap<FName, float> Stats;
TMap<EWeaponSlot, UFP_WeaponBase*> EquippedWeapons;
```

#### Добавление

```cpp
Inventory.Add(TEXT("Rifle"), 1);          // Добавить (перезапишет если ключ есть)
Inventory.Emplace(TEXT("Pistol"), 2);     // Без копирования
Inventory.FindOrAdd(TEXT("Knife"));       // Найти или создать с дефолтом
```

#### Доступ

```cpp
Inventory[TEXT("Rifle")];                  // По ключу (краш если нет!)
int32* Value = Inventory.Find(TEXT("Rifle")); // Указатель (nullptr если нет)

if (int32* Found = Inventory.Find(TEXT("Rifle")))
{
    int32 Count = *Found;
}
```

#### Проверка

```cpp
Inventory.Contains(TEXT("Rifle"));         // Есть ли ключ
Inventory.Num();                           // Количество пар
Inventory.IsEmpty();
```

#### Удаление

```cpp
Inventory.Remove(TEXT("Rifle"));           // По ключу
Inventory.Empty();                         // Очистить всё
Inventory.Reset();                         // Без освобождения памяти
```

#### Итерация

```cpp
// Range-based for
for (auto& Pair : Inventory)
{
    FString Key = Pair.Key;
    int32 Value = Pair.Value;
}

// Structured binding (C++17)
for (auto& [Key, Value] : Inventory)
{
    UE_LOG(LogTemp, Log, TEXT("%s: %d"), *Key, Value);
}

// Только ключи
TArray<FString> Keys;
Inventory.GetKeys(Keys);

// Только значения
TArray<int32> Values;
Inventory.GenerateValueArray(Values);
```

#### Особенности

```cpp
// FindOrAdd — если нет ключа, создаст с дефолтным значением
int32& AmmoCount = Inventory.FindOrAdd(TEXT("Rifle"));
AmmoCount += 10;

// Сортировка ключей
Inventory.KeySort([](const FString& A, const FString& B) {
    return A < B;
});

// Сортировка значений
Inventory.ValueSort([](int32 A, int32 B) {
    return A > B;
});
```

#### UPROPERTY

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
TMap<FString, int32> Inventory;

UPROPERTY(EditAnywhere, BlueprintReadWrite)
TMap<EWeaponType, FWeaponInfo> WeaponDatabase;
```

---

### TSet

Множество — коллекция **уникальных** элементов. Аналог `std::unordered_set`.

#### Создание и использование

```cpp
TSet<FString> Tags;

Tags.Add(TEXT("Zombie"));
Tags.Add(TEXT("Enemy"));
Tags.Add(TEXT("Zombie"));     // Игнорируется — уже есть

Tags.Num();                    // 2
Tags.Contains(TEXT("Zombie")); // true
Tags.Remove(TEXT("Enemy"));
Tags.Empty();

// Итерация
for (const FString& Tag : Tags) { /* ... */ }
```

#### Операции над множествами

```cpp
TSet<int32> A = {1, 2, 3, 4, 5};
TSet<int32> B = {3, 4, 5, 6, 7};

TSet<int32> Union = A.Union(B);          // {1,2,3,4,5,6,7} — объединение
TSet<int32> Intersect = A.Intersect(B);  // {3,4,5} — пересечение
TSet<int32> Diff = A.Difference(B);      // {1,2} — разница (в A, но не в B)

bool bSubset = A.Includes(B);            // A содержит все элементы B?
```

#### UPROPERTY

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
TSet<FName> ActiveBuffs;
```

---

### TQueue

Потокобезопасная очередь FIFO. Полезна для многопоточности.

```cpp
TQueue<FString> MessageQueue;

// Добавить (из любого потока)
MessageQueue.Enqueue(TEXT("Hello"));
MessageQueue.Enqueue(TEXT("World"));

// Достать (из любого потока)
FString Message;
if (MessageQueue.Dequeue(Message))
{
    // Message = "Hello"
}

// Посмотреть без удаления
if (MessageQueue.Peek(Message))
{
    // Message = следующий в очереди
}

MessageQueue.IsEmpty();
MessageQueue.Empty(); // Очистить
```

> ⚠️ `TQueue` нельзя использовать с UPROPERTY.

---

### TLinkedList

Односвязный список. Редко используется.

```cpp
TLinkedList<int32> Node1(10);
TLinkedList<int32> Node2(20);

Node2.LinkBefore(&Node1);

// Итерация
for (TLinkedList<int32>* It = &Node1; It; It = It->GetNextLink())
{
    int32 Value = **It;
}
```

---

### TDoubleLinkedList

Двусвязный список.

```cpp
TDoubleLinkedList<FString> List;

List.AddHead(TEXT("First"));
List.AddTail(TEXT("Last"));
List.InsertNode(TEXT("Middle"), List.GetHead()->GetNextNode());

// Итерация
for (auto* Node = List.GetHead(); Node; Node = Node->GetNextNode())
{
    FString Value = Node->GetValue();
}

List.Num();
List.Empty();
```

---

### TPair и TTuple

#### TPair

```cpp
TPair<FString, int32> WeaponAmmo;
WeaponAmmo.Key = TEXT("Rifle");
WeaponAmmo.Value = 30;

// Или
TPair<FString, int32> P(TEXT("Pistol"), 12);
```

#### TTuple

```cpp
TTuple<FString, int32, float> Info(TEXT("Rifle"), 30, 45.5f);

Info.Get<0>();   // "Rifle"
Info.Get<1>();   // 30
Info.Get<2>();   // 45.5f

auto T = MakeTuple(TEXT("Pistol"), 12, 30.0f);
```

---

### TOptional

Значение, которое **может отсутствовать**. Аналог `std::optional`.

```cpp
TOptional<int32> MaybeValue;

MaybeValue.IsSet();        // false — пока пусто

MaybeValue = 42;
MaybeValue.IsSet();        // true

int32 Val = MaybeValue.GetValue();     // 42
int32 Safe = MaybeValue.Get(0);        // 42 (или 0 если пусто)

MaybeValue.Reset();        // Снова пусто
```

#### Практика

```cpp
TOptional<FHitResult> TraceForTarget()
{
    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
    {
        return Hit;
    }
    return {}; // Пусто — ничего не нашли
}

// Использование
TOptional<FHitResult> Result = TraceForTarget();
if (Result.IsSet())
{
    AActor* HitActor = Result.GetValue().GetActor();
}
```

---

### TVariant

Значение, которое может быть **одним из нескольких типов**. Аналог `std::variant`.

```cpp
TVariant<int32, float, FString> Value;

Value.Set<int32>(42);
Value.Set<FString>(TEXT("Hello"));

if (Value.IsType<FString>())
{
    FString Str = Value.Get<FString>();
}
```

---

### TBitArray

Массив битов. Каждый элемент — 0 или 1. Экономит память.

```cpp
TBitArray<> Bits;

Bits.Init(false, 100);   // 100 битов, все = false

Bits[0] = true;
Bits[50] = true;

bool Val = Bits[0];       // true
int32 Num = Bits.Num();   // 100
```

**Когда:** флаги для огромного количества объектов (тысячи врагов, тайлы карты).

---

### TSparseArray

Массив с "дырками" — элементы можно удалять без сдвига остальных.

```cpp
TSparseArray<FString> Sparse;

int32 Idx0 = Sparse.Add(TEXT("A"));  // 0
int32 Idx1 = Sparse.Add(TEXT("B"));  // 1
int32 Idx2 = Sparse.Add(TEXT("C"));  // 2

Sparse.RemoveAt(1);                   // Удалили "B", но индексы 0 и 2 остались

Sparse.IsValidIndex(1);               // false — дырка
Sparse.IsValidIndex(2);               // true — "C"

// Итерация (пропускает дырки автоматически)
for (auto It = Sparse.CreateIterator(); It; ++It)
{
    FString Value = *It;
}
```

**Когда:** частое удаление из середины, но нужен стабильный индекс.

---

### TStaticArray

Массив фиксированного размера. Аналог `std::array`.

```cpp
TStaticArray<int32, 4> FixedArray;
FixedArray[0] = 10;
FixedArray[1] = 20;
FixedArray[2] = 30;
FixedArray[3] = 40;

FixedArray.Num(); // Всегда 4
```

---

## USTRUCT — пользовательские структуры

### Объявление USTRUCT

```cpp
USTRUCT(BlueprintType)
struct FWeaponInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 MaxAmmo = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TObjectPtr<UTexture2D> Icon = nullptr;
};
```

Обязательные части:
- `USTRUCT()` — макрос (добавь `BlueprintType` если нужна в блюпринтах)
- `struct F...` — имя обязательно начинается с **F**
- `GENERATED_BODY()` — обязательный макрос внутри

---

### USTRUCT + UPROPERTY

Спецификаторы USTRUCT:

| Спецификатор | Описание |
|---|---|
| `BlueprintType` | Доступна в блюпринтах |
| `Atomic` | Всегда сериализуется целиком |
| `NoExport` | Не генерировать автокод |

Спецификаторы UPROPERTY в структуре — такие же как в классе:

```cpp
USTRUCT(BlueprintType)
struct FPlayerStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Health = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0", ClampMax = "100"))
    float Armor = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 KillCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (UIMin = "0", UIMax = "10"))
    int32 Level = 1;
};
```

---

### Методы внутри структур

Структуры могут содержать методы, но они **не могут быть UFUNCTION**:

```cpp
USTRUCT(BlueprintType)
struct FWeaponInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentAmmo = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxAmmo = 30;

    // ✅ Обычные методы — можно
    bool HasAmmo() const { return CurrentAmmo > 0; }

    float GetAmmoPercent() const
    {
        return MaxAmmo > 0 ? (float)CurrentAmmo / (float)MaxAmmo : 0.0f;
    }

    void Refill() { CurrentAmmo = MaxAmmo; }

    bool ConsumeAmmo(int32 Amount = 1)
    {
        if (CurrentAmmo >= Amount)
        {
            CurrentAmmo -= Amount;
            return true;
        }
        return false;
    }

    // ❌ UFUNCTION внутри USTRUCT — НЕЛЬЗЯ
    // UFUNCTION(BlueprintCallable)
    // void Reload(); // Ошибка компиляции!
};
```

> **Правило:** `UFUNCTION()` работает только в `UCLASS`, не в `USTRUCT`.

---

### Конструкторы

```cpp
USTRUCT(BlueprintType)
struct FDamageInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDamageType DamageType = EDamageType::Physical;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Instigator = nullptr;

    // Конструктор по умолчанию
    FDamageInfo() = default;

    // Пользовательский конструктор
    FDamageInfo(float InDamage, EDamageType InType, AActor* InInstigator)
        : Damage(InDamage)
        , DamageType(InType)
        , Instigator(InInstigator)
    {
    }
};

// Использование
FDamageInfo Info(50.0f, EDamageType::Fire, this);
```

---

### Наследование структур

```cpp
USTRUCT(BlueprintType)
struct FBaseStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Health = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Speed = 600.0f;
};

USTRUCT(BlueprintType)
struct FZombieStats : public FBaseStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackDamage = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackRange = 150.0f;
};

// FZombieStats имеет Health, Speed, AttackDamage, AttackRange
```

---

### Структуры в блюпринтах

Чтобы структура была доступна в блюпринтах:

```cpp
// 1. Добавь BlueprintType
USTRUCT(BlueprintType)
struct FWeaponInfo
{
    GENERATED_BODY()

    // 2. Каждое поле — BlueprintReadWrite или BlueprintReadOnly
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0.0f;
};

// 3. Используй в UPROPERTY или UFUNCTION
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FWeaponInfo PrimaryWeapon;

UFUNCTION(BlueprintCallable)
void ApplyWeapon(const FWeaponInfo& Info);

UFUNCTION(BlueprintPure)
FWeaponInfo GetCurrentWeapon() const;
```

В блюпринте:
- **Make WeaponInfo** — создать структуру
- **Break WeaponInfo** — разобрать на поля
- **Set Members** — установить отдельные поля

---

### Структуры как параметры функций

```cpp
// По константной ссылке (чтение) — самый частый вариант
void PrintInfo(const FWeaponInfo& Info);

// По ссылке (чтение/запись)
void ModifyInfo(FWeaponInfo& Info);

// По значению (копия) — для маленьких структур
void ProcessInfo(FWeaponInfo Info);

// Возврат структуры
FWeaponInfo CreateDefaultWeapon();

// Возврат через параметр (out)
void GetWeaponInfo(FWeaponInfo& OutInfo);

// UFUNCTION с out-параметром
UFUNCTION(BlueprintCallable)
void GetWeaponInfo(FWeaponInfo& OutInfo);
// В блюпринте OutInfo будет выходным пином
```

---

### Data Table Row

Структура для использования в **Data Tables** — таблицах данных в редакторе:

```cpp
#include "Engine/DataTable.h"

USTRUCT(BlueprintType)
struct FWeaponTableRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxAmmo = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FireRate = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> WeaponActorClass;
};
```

Использование Data Table в C++:

```cpp
UPROPERTY(EditAnywhere, Category = "Data")
TObjectPtr<UDataTable> WeaponTable;

void LoadWeaponData(FName RowName)
{
    if (!WeaponTable) return;

    static const FString ContextString(TEXT("WeaponLookup"));
    FWeaponTableRow* Row = WeaponTable->FindRow<FWeaponTableRow>(RowName, ContextString);

    if (Row)
    {
        float Damage = Row->Damage;
        FString Name = Row->DisplayName;
    }
}

// Получить все строки
void LoadAllWeapons()
{
    if (!WeaponTable) return;

    TArray<FWeaponTableRow*> AllRows;
    static const FString ContextString(TEXT("AllWeapons"));
    WeaponTable->GetAllRows<FWeaponTableRow>(ContextString, AllRows);

    for (FWeaponTableRow* Row : AllRows)
    {
        UE_LOG(LogTemp, Log, TEXT("Weapon: %s, Damage: %f"), *Row->DisplayName, Row->Damage);
    }
}
```

---

## Сравнение: что когда использовать

### Контейнеры

| Задача | Контейнер | Почему |
|--------|-----------|--------|
| Список элементов, доступ по индексу | `TArray` | Быстрый доступ, кеш-friendly |
| Пары ключ-значение | `TMap` | Быстрый поиск по ключу |
| Уникальные элементы | `TSet` | Автоматическая уникальность |
| Очередь сообщений между потоками | `TQueue` | Потокобезопасная |
| Фиксированный размер | `TStaticArray` | Без аллокаций |
| Массив с дырками | `TSparseArray` | Стабильные индексы |
| Флаги для тысяч объектов | `TBitArray` | Экономия памяти |
| Значение может отсутствовать | `TOptional` | Явно показывает "может быть пусто" |
| Одно значение из нескольких типов | `TVariant` | Type-safe union |

### Сложность операций

| Операция | TArray | TMap | TSet |
|----------|--------|------|------|
| Доступ по индексу | O(1) | — | — |
| Поиск по ключу | — | O(1)* | O(1)* |
| Поиск значения | O(n) | O(n) | O(1)* |
| Добавление в конец | O(1)** | O(1)* | O(1)* |
| Вставка в середину | O(n) | O(1)* | O(1)* |
| Удаление из середины | O(n) | O(1)* | O(1)* |
| Удаление из конца | O(1) | — | — |

\* амортизированная, ** амортизированная, иногда O(n) при реаллокации

### Struct vs Class

| Критерий | USTRUCT | UCLASS |
|----------|---------|--------|
| Наследование | Да (ограниченное) | Да (полное) |
| UFUNCTION | ❌ Нет | ✅ Да |
| Garbage Collection | ❌ Нет (значение) | ✅ Да (указатель) |
| Блюпринт | ✅ (BlueprintType) | ✅ (Blueprintable) |
| Виртуальные методы | ❌ Нежелательно | ✅ Да |
| Указатели | По значению обычно | Всегда указатель |
| Вес | Лёгкая | Тяжёлый |

**Когда USTRUCT:** данные без сложной логики — настройки, информация, конфиги.
**Когда UCLASS:** объекты с логикой, наследованием, компонентами.

---

## Практические примеры

### Инвентарь

```cpp
USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon = nullptr;

    bool IsEmpty() const { return ItemID.IsNone() || Count <= 0; }
};

// В компоненте инвентаря
UPROPERTY(EditAnywhere, BlueprintReadWrite)
TArray<FInventorySlot> Slots;

UPROPERTY()
TMap<FName, int32> ItemCounts; // Быстрый поиск количества
```

### Система баффов

```cpp
USTRUCT(BlueprintType)
struct FBuffInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BuffID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Multiplier = 1.5f;

    float TimeRemaining = 0.0f;

    bool IsExpired() const { return TimeRemaining <= 0.0f; }
};

// Активные баффы
TArray<FBuffInfo> ActiveBuffs;

// Тик
void TickBuffs(float DeltaTime)
{
    for (FBuffInfo& Buff : ActiveBuffs)
    {
        Buff.TimeRemaining -= DeltaTime;
    }

    ActiveBuffs.RemoveAll([](const FBuffInfo& B) { return B.IsExpired(); });
}
```

### Система урона

```cpp
UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Physical,
    Fire,
    Poison,
    Explosion
};

USTRUCT(BlueprintType)
struct FDamageEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDamageType Type = EDamageType::Physical;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector HitLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector HitDirection = FVector::ZeroVector;

    UPROPERTY(BlueprintReadWrite)
    AActor* DamageCauser = nullptr;

    FDamageEvent() = default;

    FDamageEvent(float InDamage, EDamageType InType, AActor* InCauser)
        : Damage(InDamage), Type(InType), DamageCauser(InCauser) {}
};
```

### Магазин оружия

```cpp
USTRUCT(BlueprintType)
struct FShopItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Price = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UFP_WeaponDataAsset> WeaponData = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPurchased = false;

    bool CanAfford(int32 PlayerMoney) const { return PlayerMoney >= Price && !bPurchased; }
};

UPROPERTY(EditAnywhere, BlueprintReadWrite)
TArray<FShopItem> ShopItems;
```

### Настройки звуков шагов

```cpp
USTRUCT(BlueprintType)
struct FFootstepSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
    TArray<TObjectPtr<USoundBase>> WalkSounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
    TArray<TObjectPtr<USoundBase>> SprintSounds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
    float WalkInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footsteps")
    float SprintInterval = 0.3f;

    USoundBase* GetRandomWalkSound() const
    {
        return WalkSounds.IsEmpty() ? nullptr : WalkSounds[FMath::RandRange(0, WalkSounds.Num() - 1)];
    }

    USoundBase* GetRandomSprintSound() const
    {
        return SprintSounds.IsEmpty() ? nullptr : SprintSounds[FMath::RandRange(0, SprintSounds.Num() - 1)];
    }
};
```

### Состояние персонажа

```cpp
USTRUCT(BlueprintType)
struct FMovementState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsSprinting = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsAiming = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsCrouching = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsSliding = false;

    bool IsMoving() const { return bIsSprinting || bIsSliding; }
    bool CanFire() const { return !bIsSprinting && !bIsSliding; }
};
```

---

## Шпаргалка

```
┌────────────────────────────────────────────────────────────────────┐
│                    КОНТЕЙНЕРЫ UNREAL ENGINE                        │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  TArray<T>       — Динамический массив (самый частый)             │
│  TMap<K, V>      — Словарь ключ-значение                         │
│  TSet<T>         — Множество уникальных элементов                 │
│  TQueue<T>       — Потокобезопасная очередь FIFO                  │
│  TOptional<T>    — Может быть пусто                               │
│  TVariant<A,B,C> — Одно из нескольких значений                    │
│  TStaticArray    — Фиксированный размер                           │
│  TSparseArray    — С дырками, стабильные индексы                  │
│  TBitArray       — Массив битов                                   │
│                                                                    │
├────────────────────────────────────────────────────────────────────┤
│                    USTRUCT ЧЕКЛИСТ                                  │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  ✅ USTRUCT(BlueprintType)       — для блюпринтов                 │
│  ✅ GENERATED_BODY()             — обязательно                    │
│  ✅ Имя начинается с F           — FWeaponInfo, FPlayerStats      │
│  ✅ UPROPERTY в каждом поле      — для сериализации/блюпринтов    │
│  ✅ Дефолтные значения           — float Health = 100.0f          │
│  ✅ Обычные методы               — bool IsEmpty() const           │
│  ❌ UFUNCTION                    — нельзя в структурах            │
│  ❌ Виртуальные методы           — нежелательно                   │
│                                                                    │
├────────────────────────────────────────────────────────────────────┤
│                    ЧАСТЫЕ ОПЕРАЦИИ TArray                           │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  Add(Item)                — добавить                              │
│  Remove(Item)             — удалить по значению                   │
│  RemoveAt(Index)          — удалить по индексу                    │
│  RemoveAtSwap(Index)      — быстрое удаление (меняет порядок)     │
│  Contains(Item)           — проверка наличия                      │
│  Find(Item)               — индекс элемента                      │
│  FindByPredicate(Lambda)  — поиск по условию                     │
│  FilterByPredicate(Lambda)— фильтр                               │
│  Sort(Lambda)             — сортировка                            │
│  Num()                    — размер                                │
│  IsEmpty()                — пуст ли                               │
│  IsValidIndex(i)          — валиден ли индекс                    │
│  Last()                   — последний элемент                     │
│                                                                    │
├────────────────────────────────────────────────────────────────────┤
│                    ЧАСТЫЕ ОПЕРАЦИИ TMap                             │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  Add(Key, Value)          — добавить/перезаписать                 │
│  Find(Key)                — указатель на значение или nullptr     │
│  FindOrAdd(Key)           — найти или создать                     │
│  Contains(Key)            — есть ли ключ                          │
│  Remove(Key)              — удалить пару                          │
│  GetKeys(OutArray)        — все ключи                             │
│  GenerateValueArray(Out)  — все значения                          │
│  KeySort(Lambda)          — сортировать по ключам                 │
│                                                                    │
├────────────────────────────────────────────────────────────────────┤
│                    ПЕРЕДАЧА СТРУКТУР                                │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  const FMyStruct& Info    — чтение (самый частый)                 │
│  FMyStruct& OutInfo       — запись (out-параметр)                 │
│  FMyStruct Info           — копия (маленькие структуры)           │
│  FMyStruct CreateX()      — возврат по значению                   │
│                                                                    │
└────────────────────────────────────────────────────────────────────┘
```

