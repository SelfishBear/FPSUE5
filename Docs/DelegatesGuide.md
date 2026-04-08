# Делегаты в Unreal Engine — Полный гайд

## Оглавление
- [Что такое делегат](#что-такое-делегат)
- [Типы делегатов](#типы-делегатов)
- [Single Delegate (одиночный)](#single-delegate)
- [Multicast Delegate (мультикаст)](#multicast-delegate)
- [Dynamic Delegate (динамический)](#dynamic-delegate)
- [Dynamic Multicast Delegate (динамический мультикаст)](#dynamic-multicast-delegate)
- [Sparse Delegate](#sparse-delegate)
- [Привязка — все методы](#привязка--все-методы)
- [Вызов делегата](#вызов-делегата)
- [Отвязка](#отвязка)
- [Проверка состояния](#проверка-состояния)
- [Expose в Blueprint](#expose-в-blueprint)
- [Примеры использования](#примеры-использования)
- [Шпаргалка — макросы объявления](#шпаргалка--макросы-объявления)
- [Шпаргалка — когда что использовать](#шпаргалка--когда-что-использовать)

---

## Что такое делегат

Делегат — это безопасный указатель на функцию. Позволяет одному объекту **уведомить** другие объекты о событии, не зная о них напрямую.

```
Оружие стреляет → вызывает делегат → UI обновляет патроны
                                    → Камера трясётся
                                    → Звук проигрывается
```

---

## Типы делегатов

| Тип | Подписчиков | Blueprint | Сериализация |
|---|---|---|---|
| Single Delegate | 1 | ❌ | ❌ |
| Multicast Delegate | ∞ | ❌ | ❌ |
| Dynamic Delegate | 1 | ✅ | ✅ |
| Dynamic Multicast Delegate | ∞ | ✅ | ✅ |

---

## Single Delegate

Один подписчик. Самый быстрый.

### Объявление

```cpp
// Без параметров
DECLARE_DELEGATE(FOnReloadFinished);

// С параметрами
DECLARE_DELEGATE_OneParam(FOnHealthChanged, float /* NewHealth */);
DECLARE_DELEGATE_TwoParams(FOnDamageReceived, float /* Damage */, AActor* /* Instigator */);

// С возвращаемым значением
DECLARE_DELEGATE_RetVal(bool, FOnCanFire);
DECLARE_DELEGATE_RetVal_OneParam(float, FOnCalculateDamage, float /* BaseDamage */);
```

### Макросы объявления

```cpp
DECLARE_DELEGATE(FDelegateName);
DECLARE_DELEGATE_OneParam(FDelegateName, Param1Type);
DECLARE_DELEGATE_TwoParams(FDelegateName, Param1Type, Param2Type);
DECLARE_DELEGATE_ThreeParams(FDelegateName, Param1Type, Param2Type, Param3Type);
DECLARE_DELEGATE_FourParams(...);
DECLARE_DELEGATE_FiveParams(...);
DECLARE_DELEGATE_SixParams(...);
DECLARE_DELEGATE_SevenParams(...);
DECLARE_DELEGATE_EightParams(...);
DECLARE_DELEGATE_NineParams(...);

// С возвращаемым значением
DECLARE_DELEGATE_RetVal(RetValType, FDelegateName);
DECLARE_DELEGATE_RetVal_OneParam(RetValType, FDelegateName, Param1Type);
// ... до NineParams
```

### Использование

```cpp
// .h — объявление
DECLARE_DELEGATE_OneParam(FOnHealthChanged, float);

UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    FOnHealthChanged OnHealthChanged;
};

// Подписка (из другого класса)
Character->OnHealthChanged.BindUObject(this, &UHealthWidget::UpdateHealth);

// Вызов
OnHealthChanged.ExecuteIfBound(CurrentHealth);
```

---

## Multicast Delegate

Несколько подписчиков. Нет возвращаемого значения.

### Объявление

```cpp
DECLARE_MULTICAST_DELEGATE(FOnGameStarted);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32 /* NewAmmo */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWeaponFired, int32 /* AmmoLeft */, FVector /* Location */);
// ... до NineParams
```

### Использование

```cpp
// .h
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32);

class AWeapon
{
public:
    FOnAmmoChanged OnAmmoChanged;
};

// Подписка (несколько подписчиков)
Weapon->OnAmmoChanged.AddUObject(this, &UAmmoWidget::UpdateAmmo);
Weapon->OnAmmoChanged.AddUObject(this, &UAudioManager::PlayAmmoSound);

// Вызов — Broadcast вместо Execute
OnAmmoChanged.Broadcast(CurrentAmmo);
```

---

## Dynamic Delegate

Один подписчик. Работает с Blueprint. Функция должна быть `UFUNCTION()`.

### Объявление

```cpp
DECLARE_DYNAMIC_DELEGATE(FOnInteract);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDamage, float, Damage, AActor*, Instigator);

// С возвращаемым значением
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FOnCanInteract);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(float, FOnCalcDamage, float, BaseDamage);
```

> ⚠️ В Dynamic делегатах параметры указываются как `Тип, Имя` (а не просто `Тип`).

### Использование

```cpp
// .h
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);

UPROPERTY()
FOnHealthChanged OnHealthChanged;

// Подписка
OnHealthChanged.BindDynamic(this, &UMyWidget::HandleHealthChanged);

// Вызов
OnHealthChanged.ExecuteIfBound(CurrentHealth);
```

Функция-обработчик **обязана** быть `UFUNCTION()`:

```cpp
UFUNCTION()
void HandleHealthChanged(float NewHealth);
```

---

## Dynamic Multicast Delegate

Несколько подписчиков. Работает с Blueprint. **Самый популярный тип.**

### Объявление

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponEquipped, int32, SlotIndex, UFP_WeaponDataAsset*, WeaponData);
// ... до NineParams
```

### Использование

```cpp
// .h
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);

UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHealthChanged OnHealthChanged;
};

// Подписка из C++
Character->OnHealthChanged.AddDynamic(this, &UMyWidget::HandleHealthChanged);

// Вызов
OnHealthChanged.Broadcast(CurrentHealth);
```

### Blueprint

С `BlueprintAssignable` делегат появляется в Blueprint как **Event Dispatcher**:
- ПКМ на ноде персонажа → `Assign OnHealthChanged` — создаёт Custom Event
- Или в Event Graph → `Bind Event to OnHealthChanged`

---

## Sparse Delegate

Оптимизированный Dynamic Multicast — экономит память когда делегат редко используется (например компоненты).

```cpp
// Встречается в движке:
// OnComponentBeginOverlap, OnComponentHit и т.д.
// Объявляется макросами DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_*
```

Обычно не нужно создавать свои — используется в движке.

---

## Привязка — все методы

### Single Delegate

| Метод | Описание |
|---|---|
| `BindUObject(Object, &Func)` | К методу UObject |
| `BindRaw(RawPtr, &Func)` | К методу обычного C++ объекта (не UObject) |
| `BindSP(SharedPtr, &Func)` | К методу через SharedPtr |
| `BindWeakLambda(Object, Lambda)` | К лямбде с weak-ссылкой на UObject |
| `BindLambda(Lambda)` | К лямбде |
| `BindStatic(&StaticFunc)` | К статической функции |
| `BindUFunction(Object, FName)` | К UFUNCTION по имени |
| `BindDynamic(Object, &Func)` | Только для Dynamic Delegate |

### Multicast Delegate

| Метод | Возврат | Описание |
|---|---|---|
| `AddUObject(Object, &Func)` | `FDelegateHandle` | К методу UObject |
| `AddRaw(RawPtr, &Func)` | `FDelegateHandle` | К обычному C++ объекту |
| `AddSP(SharedPtr, &Func)` | `FDelegateHandle` | Через SharedPtr |
| `AddWeakLambda(Object, Lambda)` | `FDelegateHandle` | Лямбда с weak-ссылкой |
| `AddLambda(Lambda)` | `FDelegateHandle` | К лямбде |
| `AddStatic(&StaticFunc)` | `FDelegateHandle` | К статической функции |
| `AddUFunction(Object, FName)` | `FDelegateHandle` | К UFUNCTION по имени |

### Dynamic Multicast Delegate

| Метод | Описание |
|---|---|
| `AddDynamic(Object, &Func)` | Подписка (функция должна быть UFUNCTION) |
| `AddUniqueDynamic(Object, &Func)` | Подписка только если ещё не подписан |

---

## Вызов делегата

| Тип | Метод | Описание |
|---|---|---|
| Single | `Execute(...)` | Вызов (crash если не привязан) |
| Single | `ExecuteIfBound(...)` | Безопасный вызов |
| Multicast | `Broadcast(...)` | Вызов всех подписчиков |
| Dynamic | `Execute(...)` | Вызов (crash если не привязан) |
| Dynamic | `ExecuteIfBound(...)` | Безопасный вызов |
| Dynamic Multicast | `Broadcast(...)` | Вызов всех подписчиков |

---

## Отвязка

### Single Delegate

```cpp
MyDelegate.Unbind();
```

### Multicast Delegate

```cpp
// По хэндлу (рекомендуется)
FDelegateHandle Handle = OnAmmoChanged.AddUObject(this, &UWidget::Update);
OnAmmoChanged.Remove(Handle);

// Удалить все привязки конкретного объекта
OnAmmoChanged.RemoveAll(this);

// Очистить все привязки
OnAmmoChanged.Clear();
```

### Dynamic Multicast Delegate

```cpp
OnHealthChanged.RemoveDynamic(this, &UMyWidget::HandleHealthChanged);
OnHealthChanged.RemoveAll(this);
OnHealthChanged.Clear();
```

---

## Проверка состояния

| Метод | Single | Multicast | Описание |
|---|---|---|---|
| `IsBound()` | ✅ | ✅ | Есть ли хоть один подписчик |
| `IsBoundToObject(Object)` | ✅ | ❌ | Привязан ли к конкретному объекту |
| `Contains(Object, &Func)` | ❌ | ✅ (Dynamic) | Содержит ли конкретную привязку |

```cpp
if (OnHealthChanged.IsBound())
{
    OnHealthChanged.Broadcast(Health);
}

// Для Dynamic Multicast
if (!OnHealthChanged.Contains(this, &UWidget::Update))
{
    OnHealthChanged.AddDynamic(this, &UWidget::Update);
}
```

---

## Expose в Blueprint

### BlueprintAssignable — подписка из Blueprint

```cpp
UPROPERTY(BlueprintAssignable, Category = "Events")
FOnHealthChanged OnHealthChanged;
```

В Blueprint: `Bind Event to OnHealthChanged` или `Assign` нода.

### BlueprintCallable — вызов из Blueprint

```cpp
UPROPERTY(BlueprintCallable, Category = "Events")
FOnHealthChanged OnHealthChanged;
```

В Blueprint: можно вызвать `Call OnHealthChanged`.

### Оба сразу

```cpp
UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
FOnHealthChanged OnHealthChanged;
```

> ⚠️ Только `DECLARE_DYNAMIC_MULTICAST_DELEGATE` поддерживает `BlueprintAssignable`.

---

## Примеры использования

### Система здоровья

```cpp
// HealthComponent.h
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS()
class UHealthComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnDeath OnDeath;

    void ApplyDamage(float Damage);

private:
    float Health = 100.f;
    float MaxHealth = 100.f;
};

// HealthComponent.cpp
void UHealthComponent::ApplyDamage(float Damage)
{
    Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
    OnHealthChanged.Broadcast(Health, MaxHealth);

    if (Health <= 0.f)
    {
        OnDeath.Broadcast();
    }
}
```

### Подписка из HUD

```cpp
void UHealthWidget::NativeConstruct()
{
    Super::NativeConstruct();

    AMyCharacter* Character = Cast<AMyCharacter>(GetOwningPlayerPawn());
    if (Character && Character->HealthComponent)
    {
        Character->HealthComponent->OnHealthChanged.AddDynamic(this, &UHealthWidget::UpdateHealthBar);
        Character->HealthComponent->OnDeath.AddDynamic(this, &UHealthWidget::ShowDeathScreen);
    }
}

UFUNCTION()
void UHealthWidget::UpdateHealthBar(float CurrentHealth, float MaxHealth)
{
    HealthBar->SetPercent(CurrentHealth / MaxHealth);
}
```

### Оружейная система — смена оружия

```cpp
// EquipmentManager.h
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponChanged, int32, SlotIndex, UFP_WeaponBase*, NewWeapon);

UPROPERTY(BlueprintAssignable, Category = "Equipment")
FOnWeaponChanged OnWeaponChanged;

// EquipmentManager.cpp
void UFP_EquipmentManager::EquipByIndex(int32 Index)
{
    CurrentWeapon = EquippedWeapons[Index];
    CurrentWeaponIndex = Index;
    OnWeaponChanged.Broadcast(Index, CurrentWeapon);
}
```

### Делегат с лямбдой (быстрая подписка)

```cpp
// Multicast с лямбдой
OnAmmoChanged.AddWeakLambda(this, [this](int32 NewAmmo)
{
    UE_LOG(LogTemp, Warning, TEXT("Ammo: %d"), NewAmmo);
});

// Single с лямбдой
OnReloadFinished.BindLambda([this]()
{
    bIsReloading = false;
});
```

### Делегат как параметр функции

```cpp
// Передать делегат в функцию
DECLARE_DELEGATE(FOnComplete);

void LoadDataAsync(FOnComplete OnComplete)
{
    // ... загрузка ...
    OnComplete.ExecuteIfBound();
}

// Вызов
LoadDataAsync(FOnComplete::CreateUObject(this, &AMyActor::OnDataLoaded));
LoadDataAsync(FOnComplete::CreateLambda([]() { UE_LOG(LogTemp, Log, TEXT("Done!")); }));
```

---

## Шпаргалка — макросы объявления

### Single (один подписчик, без Blueprint)

```
DECLARE_DELEGATE(Name)
DECLARE_DELEGATE_OneParam(Name, P1)
DECLARE_DELEGATE_TwoParams(Name, P1, P2)
...до NineParams

DECLARE_DELEGATE_RetVal(Ret, Name)
DECLARE_DELEGATE_RetVal_OneParam(Ret, Name, P1)
...до NineParams
```

### Multicast (много подписчиков, без Blueprint)

```
DECLARE_MULTICAST_DELEGATE(Name)
DECLARE_MULTICAST_DELEGATE_OneParam(Name, P1)
...до NineParams
```

### Dynamic (один подписчик, Blueprint)

```
DECLARE_DYNAMIC_DELEGATE(Name)
DECLARE_DYNAMIC_DELEGATE_OneParam(Name, P1Type, P1Name)
...до NineParams

DECLARE_DYNAMIC_DELEGATE_RetVal(Ret, Name)
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(Ret, Name, P1Type, P1Name)
...до NineParams
```

### Dynamic Multicast (много подписчиков, Blueprint) — САМЫЙ ЧАСТЫЙ

```
DECLARE_DYNAMIC_MULTICAST_DELEGATE(Name)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(Name, P1Type, P1Name)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(Name, P1Type, P1Name, P2Type, P2Name)
...до NineParams
```

---

## Шпаргалка — когда что использовать

| Ситуация | Тип делегата |
|---|---|
| C++ → C++, один подписчик | `DECLARE_DELEGATE` |
| C++ → C++, много подписчиков | `DECLARE_MULTICAST_DELEGATE` |
| C++ → Blueprint, один подписчик | `DECLARE_DYNAMIC_DELEGATE` |
| C++ → Blueprint, много подписчиков | `DECLARE_DYNAMIC_MULTICAST_DELEGATE` |
| Event Dispatcher в Blueprint | `DECLARE_DYNAMIC_MULTICAST_DELEGATE` + `BlueprintAssignable` |
| Нужна максимальная производительность | `DECLARE_DELEGATE` или `DECLARE_MULTICAST_DELEGATE` |
| Нужен возврат значения | `DECLARE_DELEGATE_RetVal` или `DECLARE_DYNAMIC_DELEGATE_RetVal` |
| Параметр функции (callback) | `DECLARE_DELEGATE` |

