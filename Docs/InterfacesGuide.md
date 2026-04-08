# Интерфейсы в Unreal Engine — Полный гайд

## Оглавление
- [Что такое интерфейс](#что-такое-интерфейс)
- [Создание интерфейса в C++](#создание-интерфейса-в-c)
- [Реализация интерфейса](#реализация-интерфейса)
- [Вызов функций интерфейса](#вызов-функций-интерфейса)
- [Проверка реализации](#проверка-реализации)
- [BlueprintNativeEvent — C++ + Blueprint](#blueprintnativeevent--c--blueprint)
- [BlueprintImplementableEvent — только Blueprint](#blueprintimplementableevent--только-blueprint)
- [Интерфейс в Blueprint](#интерфейс-в-blueprint)
- [Примеры использования](#примеры-использования)
- [Интерфейс vs Делегат vs Каст](#интерфейс-vs-делегат-vs-каст)
- [Шпаргалка](#шпаргалка)

---

## Что такое интерфейс

Интерфейс — контракт, который говорит: "любой класс реализующий меня, **обязан иметь** эти функции".

Используется когда разные классы должны реагировать одинаково, но ты **не знаешь** конкретный тип:

```
Враг    → реализует IInteractable → Interact() → начинает бой
Дверь   → реализует IInteractable → Interact() → открывается
Предмет → реализует IInteractable → Interact() → подбирается
```

---

## Создание интерфейса в C++

Интерфейс в UE состоит из **двух классов**: `UInterface` и `IInterface`.

### Файл .h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FP_Interactable.generated.h"

// UInterface — нужен для рефлексии, НЕ трогать
UINTERFACE(MinimalAPI, Blueprintable)
class UFP_Interactable : public UInterface
{
    GENERATED_BODY()
};

// IInterface — здесь объявляются функции
class FPSGAME_API IFP_Interactable
{
    GENERATED_BODY()

public:
    // Чистый C++ метод
    virtual void Interact(AActor* Caller) = 0;

    // Метод с дефолтной реализацией
    virtual FString GetInteractionText() const { return TEXT("Interact"); }

    // Метод с возвращаемым значением
    virtual bool CanInteract(AActor* Caller) const { return true; }
};
```

> ⚠️ `UFP_Interactable` — для рефлексии. `IFP_Interactable` — для методов. Именование **обязательно** `U` и `I` + одно имя.

---

## Реализация интерфейса

### В .h класса

```cpp
#pragma once

#include "FP_Interactable.h"

UCLASS()
class AMyDoor : public AActor, public IFP_Interactable  // ← наследование от IInterface
{
    GENERATED_BODY()

public:
    // Реализация методов интерфейса
    virtual void Interact(AActor* Caller) override;
    virtual FString GetInteractionText() const override;
    virtual bool CanInteract(AActor* Caller) const override;
};
```

### В .cpp

```cpp
void AMyDoor::Interact(AActor* Caller)
{
    bIsOpen = !bIsOpen;
    // открыть/закрыть дверь
}

FString AMyDoor::GetInteractionText() const
{
    return bIsOpen ? TEXT("Close Door") : TEXT("Open Door");
}

bool AMyDoor::CanInteract(AActor* Caller) const
{
    return !bIsLocked;
}
```

---

## Вызов функций интерфейса

### Способ 1: Cast к интерфейсу (рекомендуется для C++)

```cpp
AActor* HitActor = HitResult.GetActor();

IFP_Interactable* Interactable = Cast<IFP_Interactable>(HitActor);
if (Interactable)
{
    if (Interactable->CanInteract(this))
    {
        Interactable->Interact(this);
    }
}
```

### Способ 2: TScriptInterface (для UPROPERTY)

```cpp
UPROPERTY()
TScriptInterface<IFP_Interactable> CurrentInteractable;

// Присвоение
CurrentInteractable = HitActor;

// Вызов
if (CurrentInteractable)
{
    IFP_Interactable::Execute_Interact(CurrentInteractable.GetObject(), this);
}
```

### Способ 3: Execute_ статические функции (для BlueprintNativeEvent)

```cpp
// Для функций объявленных как BlueprintNativeEvent
IFP_Damageable::Execute_TakeDamage(HitActor, 25.f, this);
```

> Подробнее в разделе [BlueprintNativeEvent](#blueprintnativeevent--c--blueprint).

---

## Проверка реализации

### Implements<>() — для UObject

```cpp
AActor* Actor = HitResult.GetActor();

if (Actor && Actor->Implements<UFP_Interactable>())  // ← UFP (U-класс!)
{
    IFP_Interactable* Interactable = Cast<IFP_Interactable>(Actor);
    Interactable->Interact(this);
}
```

### Cast<> — одновременно проверка и получение указателя

```cpp
if (IFP_Interactable* Interactable = Cast<IFP_Interactable>(Actor))
{
    Interactable->Interact(this);
}
```

### Проверка в TArray

```cpp
for (AActor* Actor : FoundActors)
{
    if (Actor->Implements<UFP_Interactable>())
    {
        IFP_Interactable::Execute_Interact(Actor, this);
    }
}
```

---

## BlueprintNativeEvent — C++ + Blueprint

Функция с **дефолтной реализацией в C++**, которую **можно переопределить в Blueprint**.

### Объявление

```cpp
UINTERFACE(MinimalAPI, Blueprintable)
class UFP_Damageable : public UInterface
{
    GENERATED_BODY()
};

class FPSGAME_API IFP_Damageable
{
    GENERATED_BODY()

public:
    // BlueprintNativeEvent — C++ реализация + Blueprint переопределение
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
    void TakeDamage(float Amount, AActor* Instigator);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
    bool IsDead() const;
};
```

### Реализация в C++ (суффикс _Implementation)

```cpp
// В .h класса
UCLASS()
class AMyEnemy : public ACharacter, public IFP_Damageable
{
    GENERATED_BODY()

public:
    virtual void TakeDamage_Implementation(float Amount, AActor* Instigator) override;
    virtual bool IsDead_Implementation() const override;
};

// В .cpp
void AMyEnemy::TakeDamage_Implementation(float Amount, AActor* Instigator)
{
    Health -= Amount;
    if (Health <= 0.f)
    {
        Die();
    }
}

bool AMyEnemy::IsDead_Implementation() const
{
    return Health <= 0.f;
}
```

### Вызов — ОБЯЗАТЕЛЬНО через Execute_

```cpp
// ✅ Правильно — вызовет Blueprint версию если она есть
IFP_Damageable::Execute_TakeDamage(EnemyActor, 25.f, this);

// ❌ Неправильно — пропустит Blueprint переопределение
IFP_Damageable* Damageable = Cast<IFP_Damageable>(EnemyActor);
Damageable->TakeDamage(25.f, this); // НЕ вызовет Blueprint!
```

> `Execute_` — статическая функция, автоматически сгенерированная UHT. Она правильно вызывает Blueprint версию.

---

## BlueprintImplementableEvent — только Blueprint

Функция **без** реализации в C++. Реализуется только в Blueprint.

### Объявление

```cpp
class FPSGAME_API IFP_QuestObjective
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Quest")
    void OnObjectiveComplete();

    UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
    FString GetObjectiveDescription();
};
```

### Вызов из C++

```cpp
// Также через Execute_
IFP_QuestObjective::Execute_OnObjectiveComplete(Actor);
FString Desc = IFP_QuestObjective::Execute_GetObjectiveDescription(Actor);
```

### Реализация — только в Blueprint

В Blueprint классе, реализующем интерфейс, появятся функции в `My Blueprint → Interfaces` → их можно имплементировать двойным кликом.

---

## Интерфейс в Blueprint

### Создание

`Add → Blueprint Class → Blueprint Interface`

Или в Content Browser: `ПКМ → Blueprints → Blueprint Interface`

### Добавление функций

В редакторе интерфейса: `+ Add Function` → задать имя, параметры, возврат.

### Реализация в Blueprint классе

`Class Settings → Interfaces → Add → выбрать интерфейс`

Функции появятся в `My Blueprint → Interfaces`.

### Вызов в Blueprint

```
Нода: Does Implement Interface → проверка
Нода: Message (имя функции) → вызов через интерфейс (правый клик на функцию → Call Function)
```

Или просто перетащить функцию интерфейса — если актор реализует, вызовется.

---

## Примеры использования

### Система взаимодействия

```cpp
// FP_Interactable.h
UINTERFACE(MinimalAPI, Blueprintable)
class UFP_Interactable : public UInterface { GENERATED_BODY() };

class FPSGAME_API IFP_Interactable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(AActor* Caller);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FText GetInteractionText() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    bool CanInteract(AActor* Caller) const;
};

// Персонаж — Line Trace + вызов
void AFP_Character::TryInteract()
{
    FHitResult Hit;
    FVector Start = CameraComponent->GetComponentLocation();
    FVector End = Start + CameraComponent->GetForwardVector() * InteractDistance;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->Implements<UFP_Interactable>())
        {
            bool bCan = IFP_Interactable::Execute_CanInteract(HitActor, this);
            if (bCan)
            {
                IFP_Interactable::Execute_Interact(HitActor, this);
            }
        }
    }
}
```

### Система урона

```cpp
// FP_Damageable.h
UINTERFACE(MinimalAPI, Blueprintable)
class UFP_Damageable : public UInterface { GENERATED_BODY() };

class FPSGAME_API IFP_Damageable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void ApplyDamage(float Damage, AActor* DamageCauser, AController* Instigator);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    float GetCurrentHealth() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool IsAlive() const;
};

// Оружие — нанесение урона
void AWeapon::OnHit(FHitResult& Hit)
{
    AActor* HitActor = Hit.GetActor();
    if (HitActor && HitActor->Implements<UFP_Damageable>())
    {
        AController* OwnerController = Cast<APawn>(GetOwner())->GetController();
        IFP_Damageable::Execute_ApplyDamage(HitActor, WeaponDamage, this, OwnerController);
    }
}

// Враг
void AEnemy::ApplyDamage_Implementation(float Damage, AActor* DamageCauser, AController* Instigator)
{
    Health -= Damage;
    PlayHitReaction();
    if (Health <= 0.f)
    {
        Die();
    }
}
```

### Система подбираемых предметов

```cpp
// FP_Pickupable.h
UINTERFACE(MinimalAPI, Blueprintable)
class UFP_Pickupable : public UInterface { GENERATED_BODY() };

class FPSGAME_API IFP_Pickupable
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnPickedUp(AActor* Picker);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    FText GetItemName() const;
};

// Патроны
class AAmmoPickup : public AActor, public IFP_Pickupable
{
    virtual void OnPickedUp_Implementation(AActor* Picker) override
    {
        if (IFP_Interactable* Weapon = Cast<IFP_Interactable>(Picker))
        {
            // добавить патроны
        }
        Destroy();
    }
};
```

### Множественная реализация (класс реализует несколько интерфейсов)

```cpp
UCLASS()
class AWeaponPickup : public AActor,
                       public IFP_Interactable,    // ← можно взаимодействовать
                       public IFP_Pickupable,      // ← можно подобрать
                       public IFP_Highlightable    // ← можно подсвечивать
{
    GENERATED_BODY()

public:
    virtual void Interact_Implementation(AActor* Caller) override;
    virtual void OnPickedUp_Implementation(AActor* Picker) override;
    virtual void SetHighlight_Implementation(bool bEnable) override;
};
```

---

## Интерфейс vs Делегат vs Каст

| Критерий | Интерфейс | Делегат | Прямой Cast |
|---|---|---|---|
| Связь | Слабая | Слабая | Сильная |
| Знание о типе | Не нужно | Не нужно | Нужно |
| Направление | Вызывающий → цель | Источник → подписчик | Вызывающий → цель |
| Подписчиков | 1 (конкретный объект) | 1 или ∞ | 1 |
| Когда использовать | "Умеешь ли ты X?" | "Событие произошло" | Точно знаешь тип |

### Примеры

```cpp
// Интерфейс — "Я не знаю что ты, но если ты Damageable — получи урон"
if (Actor->Implements<UFP_Damageable>())
    IFP_Damageable::Execute_ApplyDamage(Actor, 10.f, this, nullptr);

// Делегат — "Что-то произошло, все кому интересно — реагируйте"
OnHealthChanged.Broadcast(NewHealth, MaxHealth);

// Cast — "Я точно знаю что это враг"
AEnemy* Enemy = Cast<AEnemy>(Actor);
if (Enemy) Enemy->TakeDamage(10.f);
```

---

## Шпаргалка

### Создание интерфейса

```cpp
// 1. Объявить UInterface (рефлексия)
UINTERFACE(MinimalAPI, Blueprintable)
class UMyInterface : public UInterface { GENERATED_BODY() };

// 2. Объявить IInterface (методы)
class IMyInterface
{
    GENERATED_BODY()
public:
    // Чистый виртуальный (только C++)
    virtual void MyFunc() = 0;

    // С дефолтом (только C++)
    virtual void MyFunc2() { }

    // BlueprintNativeEvent (C++ + Blueprint)
    UFUNCTION(BlueprintNativeEvent)
    void MyFunc3();

    // BlueprintImplementableEvent (только Blueprint)
    UFUNCTION(BlueprintImplementableEvent)
    void MyFunc4();
};
```

### Реализация

```cpp
class AMyActor : public AActor, public IMyInterface
{
    virtual void MyFunc() override;             // чистый виртуальный
    virtual void MyFunc2() override;            // с дефолтом
    virtual void MyFunc3_Implementation() override; // BlueprintNativeEvent
    // MyFunc4 — реализуется в Blueprint
};
```

### Вызов

```cpp
// Чистый виртуальный / с дефолтом
IMyInterface* I = Cast<IMyInterface>(Actor);
if (I) I->MyFunc();

// BlueprintNativeEvent / BlueprintImplementableEvent
IMyInterface::Execute_MyFunc3(Actor);
IMyInterface::Execute_MyFunc4(Actor);
```

### Проверка

```cpp
Actor->Implements<UMyInterface>()     // bool
Cast<IMyInterface>(Actor)             // nullptr если не реализует
```

### UINTERFACE спецификаторы

| Спецификатор | Описание |
|---|---|
| `MinimalAPI` | Не экспортировать всё, минимум для работы |
| `Blueprintable` | Можно реализовать в Blueprint |
| `NotBlueprintable` | Нельзя реализовать в Blueprint |
| `BlueprintType` | Можно использовать как тип переменной в Blueprint |
| `Meta=(CannotImplementInterfaceInBlueprint)` | Запретить реализацию в Blueprint |

### Функции интерфейса — спецификаторы

| Спецификатор | Описание |
|---|---|
| `BlueprintNativeEvent` | C++ реализация + Blueprint может переопределить |
| `BlueprintImplementableEvent` | Только Blueprint реализация |
| `BlueprintCallable` | Можно вызвать из Blueprint |

