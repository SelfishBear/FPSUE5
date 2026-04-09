# Gameplay Ability System (GAS) — Полный гайд

## Оглавление
- [Что такое GAS и зачем он нужен](#что-такое-gas-и-зачем-он-нужен)
- [Аналогия: карточная игра](#аналогия-карточная-игра)
- [Архитектура — все компоненты GAS](#архитектура--все-компоненты-gas)
- [1. Ability System Component (ASC)](#1-ability-system-component-asc)
- [2. Gameplay Abilities (GA)](#2-gameplay-abilities-ga)
- [3. Gameplay Tags](#3-gameplay-tags)
- [4. Gameplay Effects (GE)](#4-gameplay-effects-ge)
- [5. Attribute Set](#5-attribute-set)
- [6. Gameplay Cues](#6-gameplay-cues)
- [7. Gameplay Tasks](#7-gameplay-tasks)
- [8. Ability Tasks](#8-ability-tasks)
- [9. Gameplay Events](#9-gameplay-events)
- [10. Target Data и Targeting](#10-target-data-и-targeting)
- [Настройка проекта — подключение GAS](#настройка-проекта--подключение-gas)
- [Шаг за шагом: первая способность](#шаг-за-шагом-первая-способность)
- [Практический пример: FPS способности врагов](#практический-пример-fps-способности-врагов)
- [Attribute Set — подробный разбор](#attribute-set--подробный-разбор)
- [Gameplay Effects — подробный разбор](#gameplay-effects--подробный-разбор)
- [Gameplay Tags — подробный разбор](#gameplay-tags--подробный-разбор)
- [Gameplay Cues — визуальные и звуковые эффекты](#gameplay-cues--визуальные-и-звуковые-эффекты)
- [Ability Tasks — асинхронные действия внутри способностей](#ability-tasks--асинхронные-действия-внутри-способностей)
- [Networking / Мультиплеер](#networking--мультиплеер)
- [Архитектурные паттерны и лучшие практики](#архитектурные-паттерны-и-лучшие-практики)
- [Дерево решений: нужен ли GAS?](#дерево-решений-нужен-ли-gas)
- [Частые ошибки](#частые-ошибки)
- [Шпаргалка — основные классы](#шпаргалка--основные-классы)

---

## Что такое GAS и зачем он нужен

**Gameplay Ability System** — это фреймворк Unreal Engine для реализации **способностей, эффектов, атрибутов и взаимодействий**. Был создан Epic для Fortnite и Paragon, затем открыт для всех.

### Проблема без GAS

Представь, что у тебя враги с разными атаками:

```
Зомби → Взрыв (урон, разлёт, VFX, звук, кулдаун 10 сек)
Зомби → Ядовитый плевок (урон, DOT яд 5 сек, проджектайл, VFX)
Зомби → Крик (замедление игрока, дебаф на 3 сек, радиус)
Игрок → Граната (урон, радиус, VFX, отбрасывание)
Игрок → Рывок (передвижение, неуязвимость 0.5 сек, кулдаун)
```

Без системы ты пишешь **всё вручную** для каждой способности: кулдауны, стоимость маны, условия активации, отмену, сетевую репликацию... И это превращается в спагетти.

### Что даёт GAS

| Задача | Без GAS | С GAS |
|---|---|---|
| Кулдаун | Таймер вручную для каждой способности | `CooldownGameplayEffect` — одна строка |
| Стоимость (мана) | Проверка и вычитание вручную | `CostGameplayEffect` — автоматически |
| Урон + DOT (яд) | Свой таймер, своя логика | `GameplayEffect` с `Duration` и `Period` |
| Бафы / Дебафы | Свои структуры, свои массивы | `GameplayEffect` + `Modifier` |
| Условия активации | `if` на каждое условие | `Tags`: `ActivationBlockedTags`, `ActivationRequiredTags` |
| Визуальные эффекты | Вручную спавнить Niagara/Sound | `GameplayCue` — отделён от логики |
| Мультиплеер | Репликация вручную | Встроенное предсказание и подтверждение |
| Отмена способности | Свои флаги | `CancelAbilitiesWithTag` — автоматически |

---

## Аналогия: карточная игра

Представь GAS как **коллекционную карточную игру**:

| GAS компонент | Аналогия |
|---|---|
| **AbilitySystemComponent** | Игровое поле — без него нельзя играть |
| **GameplayAbility** | Карта в руке — конкретное действие (Огненный шар, Лечение) |
| **AttributeSet** | Характеристики персонажа на карточке — HP, Мана, Урон |
| **GameplayEffect** | Эффект карты — «нанести 50 урона», «лечить 10 HP/сек 5 секунд» |
| **GameplayTags** | Ключевые слова на карте — «Огонь», «Магия», «Оглушение» |
| **GameplayCue** | Анимация / звук при розыгрыше карты — спецэффекты |
| **AbilityTask** | Подэтап розыгрыша — «дождаться выбора цели», «долететь до врага» |
| **TargetData** | Выбранная цель для карты |
| **GameplayEvent** | Триггер — «когда HP ниже 20% — активировать способность» |

---

## Архитектура — все компоненты GAS

```
┌─────────────────────────────────────────────────────────────────┐
│                         ACTOR (Враг / Игрок)                    │
│                                                                 │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │           Ability System Component (ASC)                  │   │
│  │                                                           │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐  │   │
│  │  │ Attribute   │  │ Granted     │  │ Active Gameplay │  │   │
│  │  │ Set         │  │ Abilities   │  │ Effects         │  │   │
│  │  │             │  │             │  │                  │  │   │
│  │  │ HP: 100     │  │ GA_Explode  │  │ GE_Poison (DOT) │  │   │
│  │  │ Mana: 50    │  │ GA_Spit     │  │ GE_SpeedBuff    │  │   │
│  │  │ Speed: 600  │  │ GA_Scream   │  │ GE_Cooldown     │  │   │
│  │  └─────────────┘  └─────────────┘  └─────────────────┘  │   │
│  │                                                           │   │
│  │  Owned Tags: [State.Dead, Debuff.Poison, Ability.Active] │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ┌──────────────────┐                                           │
│  │ Gameplay Cue     │  ← VFX / SFX (отделены от логики)        │
│  │ Manager          │                                           │
│  └──────────────────┘                                           │
└─────────────────────────────────────────────────────────────────┘
```

---

## 1. Ability System Component (ASC)

**Центральный компонент**. Без него GAS не работает. Ставится на актора (персонаж, враг, турель — всё что имеет способности/атрибуты).

### Что делает

- Хранит список **выданных способностей** (Granted Abilities)
- Хранит активные **GameplayEffects**
- Хранит **AttributeSets**
- Управляет **GameplayTags** актора
- Обрабатывает **активацию, отмену, завершение** способностей
- Занимается **репликацией** по сети

### Создание

```cpp
// В заголовке персонажа
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

UCLASS()
class AMyCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AMyCharacter();

    // Интерфейс — обязательная реализация
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UMyAttributeSet* AttributeSet;
};
```

```cpp
// В .cpp
AMyCharacter::AMyCharacter()
{
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    AttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("Attributes"));
}

UAbilitySystemComponent* AMyCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
```

### IAbilitySystemInterface

Интерфейс `IAbilitySystemInterface` **обязателен**. Без него другие системы GAS не смогут найти ASC на акторе. Реализуешь один метод — `GetAbilitySystemComponent()`.

### Где размещать ASC

| Вариант | Описание | Когда использовать |
|---|---|---|
| На самом персонаже | ASC живёт на Pawn/Character | Одиночная игра, AI-враги |
| На PlayerState | ASC живёт на PlayerState | Мультиплеер (персонаж может респавниться, а PlayerState остаётся) |

Для **AI-врагов** — всегда на самом акторе. Для **игрока в мультиплеере** — на PlayerState.

---

## 2. Gameplay Abilities (GA)

**Конкретное действие** — атака, рывок, взрыв, лечение, блок. Наследуется от `UGameplayAbility`.

### Жизненный цикл способности

```
TryActivateAbility()
    │
    ▼
CanActivateAbility()          ← Проверка: теги, кулдаун, стоимость
    │ (если false → отказ)
    ▼
ActivateAbility()             ← ТВОЯ ЛОГИКА ЗДЕСЬ
    │
    ├── CommitAbility()       ← Применить стоимость и кулдаун
    │       ├── ApplyCost()
    │       └── ApplyCooldown()
    │
    ├── (Ability Tasks...)    ← Асинхронные действия (анимация, ожидание)
    │
    ▼
EndAbility()                  ← Завершение (обязательно вызвать!)
```

### Создание способности

```cpp
#pragma once

#include "Abilities/GameplayAbility.h"
#include "GA_EnemyExplode.generated.h"

UCLASS()
class UGA_EnemyExplode : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_EnemyExplode();

    // Можно ли активировать?
    virtual bool CanActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags = nullptr,
        const FGameplayTagContainer* TargetTags = nullptr,
        OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
    ) const override;

    // Главная логика
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Explode")
    float ExplosionRadius = 500.f;

    UPROPERTY(EditDefaultsOnly, Category = "Explode")
    float ExplosionDamage = 100.f;

    UPROPERTY(EditDefaultsOnly, Category = "Explode")
    TSubclassOf<UGameplayEffect> DamageEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Explode")
    UAnimMontage* ExplodeMontage;
};
```

```cpp
// В .cpp
UGA_EnemyExplode::UGA_EnemyExplode()
{
    // Настройки активации
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    // Теги
    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Enemy.Explode")));

    // Блокировка — нельзя взорваться если уже мёртв
    ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
}

void UGA_EnemyExplode::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    // Проверяем и применяем стоимость / кулдаун
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true); // Cancelled
        return;
    }

    // Находим всех в радиусе
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!AvatarActor) return;

    TArray<FOverlapResult> Overlaps;
    FCollisionShape Shape = FCollisionShape::MakeSphere(ExplosionRadius);

    AvatarActor->GetWorld()->OverlapMultiByChannel(
        Overlaps,
        AvatarActor->GetActorLocation(),
        FQuat::Identity,
        ECC_Pawn,
        Shape
    );

    // Применяем GameplayEffect урона к каждому найденному
    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* HitActor = Overlap.GetActor();
        if (HitActor && HitActor != AvatarActor)
        {
            if (UAbilitySystemComponent* TargetASC =
                UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
            {
                FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffect);
                ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
                    SpecHandle, TargetASC);
            }
        }
    }

    // Завершаем способность
    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
```

### Instancing Policy — как создаётся экземпляр

| Политика | Описание | Когда использовать |
|---|---|---|
| `NonInstanced` | Один объект на всех. Нельзя хранить состояние | Пассивные способности |
| `InstancedPerActor` | По экземпляру на каждого актора | **Рекомендуется по умолчанию** |
| `InstancedPerExecution` | Новый экземпляр на каждую активацию | Редко, когда несколько одновременно |

> ⚡ **Всегда используй `InstancedPerActor`** если не уверен.

### Net Execution Policy — сетевая политика

| Политика | Описание |
|---|---|
| `LocalPredicted` | Клиент начинает, сервер подтверждает (стандарт для игрока) |
| `LocalOnly` | Только на клиенте (косметические эффекты) |
| `ServerInitiated` | Сервер запускает, клиент получает (AI-враги) |
| `ServerOnly` | Только сервер (чит-защита, критичные действия) |

Для **AI-врагов** обычно `ServerInitiated` или `ServerOnly`.

### Выдача способности актору

```cpp
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        // Выдаём способность (класс задаётся в Blueprint/DataAsset)
        for (TSubclassOf<UGameplayAbility>& Ability : DefaultAbilities)
        {
            AbilitySystemComponent->GiveAbility(
                FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this)
            );
        }
    }
}
```

### Активация способности

```cpp
// По классу
AbilitySystemComponent->TryActivateAbilityByClass(UGA_EnemyExplode::StaticClass());

// По тегу
FGameplayTagContainer TagContainer;
TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Enemy.Explode")));
AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);

// По GameplayEvent (триггер)
FGameplayEventData EventData;
EventData.Instigator = this;
UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
    EnemyActor,
    FGameplayTag::RequestGameplayTag(FName("Event.Enemy.LowHealth")),
    EventData
);
```

---

## 3. Gameplay Tags

**Иерархическая система меток** — основа всей логики GAS. Это как ключевые слова / категории.

### Примеры тегов

```
State.Dead
State.Stunned
State.Invulnerable

Ability.Enemy.Explode
Ability.Enemy.PoisonSpit
Ability.Enemy.Scream
Ability.Player.Dash
Ability.Player.Grenade

Damage.Physical
Damage.Poison
Damage.Fire

Debuff.Slow
Debuff.Poison
Debuff.Stun

Cooldown.Ability.Explode
Cooldown.Ability.PoisonSpit
```

### Как теги управляют способностями

```cpp
UGA_EnemyExplode::UGA_EnemyExplode()
{
    // Эта способность имеет тег
    AbilityTags.AddTag(TAG("Ability.Enemy.Explode"));

    // Нельзя активировать, если на акторе есть эти теги
    ActivationBlockedTags.AddTag(TAG("State.Dead"));
    ActivationBlockedTags.AddTag(TAG("State.Stunned"));

    // Нужны эти теги для активации
    ActivationRequiredTags.AddTag(TAG("State.Enraged"));

    // При активации отменить другие способности с этими тегами
    CancelAbilitiesWithTag.AddTag(TAG("Ability.Enemy.Movement"));

    // Пока активна — блокировать другие способности с этими тегами
    BlockAbilitiesWithTag.AddTag(TAG("Ability.Enemy"));
}
```

### Таблица тегов способности

| Свойство | Описание |
|---|---|
| `AbilityTags` | Теги самой способности (для поиска и идентификации) |
| `ActivationRequiredTags` | Теги которые ДОЛЖНЫ быть на акторе для активации |
| `ActivationBlockedTags` | Теги которые БЛОКИРУЮТ активацию |
| `CancelAbilitiesWithTag` | При активации отменить способности с этими тегами |
| `BlockAbilitiesWithTag` | Пока активна, блокировать способности с этими тегами |
| `SourceRequiredTags` | Теги на источнике (кастере) |
| `SourceBlockedTags` | Теги-блокеры на источнике |
| `TargetRequiredTags` | Теги на цели |
| `TargetBlockedTags` | Теги-блокеры на цели |

### Регистрация тегов

**Способ 1: через .ini файл** (DefaultGameplayTags.ini)
```ini
[/Script/GameplayTags.GameplayTagsSettings]
+GameplayTagList=(Tag="State.Dead",DevComment="")
+GameplayTagList=(Tag="State.Stunned",DevComment="")
+GameplayTagList=(Tag="Ability.Enemy.Explode",DevComment="")
```

**Способ 2: через DataTable** — создать DataTable с Row Type = GameplayTagTableRow

**Способ 3: в C++ через FNativeGameplayTag** (рекомендуется для кода)
```cpp
// В .h
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_Dead);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_Explode);

// В .cpp
UE_DEFINE_GAMEPLAY_TAG(TAG_State_Dead, "State.Dead");
UE_DEFINE_GAMEPLAY_TAG(TAG_Ability_Explode, "Ability.Enemy.Explode");
```

---

## 4. Gameplay Effects (GE)

**Эффект — это данные**, описывающие **что** происходит с атрибутами. Урон, лечение, бафы, дебафы, DOT — всё это GameplayEffect.

### Типы эффектов по длительности

| Тип | Описание | Пример |
|---|---|---|
| `Instant` | Применяется мгновенно и навсегда | Урон, лечение |
| `Duration` | Длится заданное время | Баф скорости на 10 сек |
| `Infinite` | Длится пока не удалишь вручную | Пассивная аура, экипировка |

### Создание (Blueprint — самый удобный способ)

1. **Content Browser** → ПКМ → **Blueprint Class** → **GameplayEffect**
2. Назвать `GE_ExplosionDamage`
3. Открыть и настроить:

```
Duration Policy: Instant

Modifiers:
  [0] Attribute: MyAttributeSet.Health
      Modifier Op: Add
      Modifier Magnitude: Scalable Float = -100   (минус = урон)

Gameplay Cues:
  [0] GameplayCue.Ability.Explode    (VFX/SFX)

Tags:
  Asset Tag: Damage.Physical
```

### Создание в C++ (для продвинутых кейсов)

```cpp
UCLASS()
class UGE_PoisonDOT : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UGE_PoisonDOT()
    {
        DurationPolicy = EGameplayEffectDurationType::HasDuration;
        DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(5.f)); // 5 секунд

        // Периодический урон каждую секунду
        Period = FScalableFloat(1.f);

        // Модификатор
        FGameplayModifierInfo DamageModifier;
        DamageModifier.Attribute = UMyAttributeSet::GetHealthAttribute();
        DamageModifier.ModifierOp = EGameplayModOp::Additive;
        DamageModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-10.f));
        Modifiers.Add(DamageModifier);
    }
};
```

> ⚡ **На практике Gameplay Effects почти всегда создают в Blueprint** — это удобнее, нагляднее, и дизайнеры могут менять значения без перекомпиляции.

### Применение эффекта

```cpp
// Из GameplayAbility
FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());

// Установить дополнительные данные через SetByCaller
FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, 75.f);

// Применить к цели
ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
    SpecHandle, TargetASC);

// Применить к себе
ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
    SpecHandle);
```

### Modifier Operations

| Операция | Описание | Формула |
|---|---|---|
| `Add` | Прибавить | `Base + Value` |
| `Multiply` | Умножить | `Base * Value` |
| `Divide` | Разделить | `Base / Value` |
| `Override` | Заменить | `Value` |

### Stacking — наложение эффектов

Можно настроить, как одинаковые эффекты складываются:

```
Stacking Type: Aggregate by Source / Aggregate by Target
Stack Limit: 5                  (максимум 5 стаков яда)
Stack Duration Refresh Policy: RefreshOnSuccessfulApplication
Stack Period Reset Policy: ResetOnSuccessfulApplication
```

Пример: яд стакается до 5 раз, каждый стак добавляет -10 HP/сек. Итого при 5 стаках = -50 HP/сек.

---

## 5. Attribute Set

**Набор числовых характеристик** актора: HP, мана, скорость, урон, броня и т.д.

### Создание

```cpp
#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MyAttributeSet.generated.h"

// Макрос для удобного доступа к атрибутам
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class UMyAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UMyAttributeSet();

    // Здоровье
    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxHealth)

    // Мана / Энергия
    UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, Mana)

    UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxMana)

    // Скорость
    UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, MoveSpeed)

    // Мета-атрибут: входящий урон (временный, не реплицируется)
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    FGameplayAttributeData IncomingDamage;
    ATTRIBUTE_ACCESSORS(UMyAttributeSet, IncomingDamage)

    // Вызывается ПЕРЕД изменением атрибута — можно зажать (clamp)
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    // Вызывается ПОСЛЕ применения эффекта — основная логика
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    // Репликация
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
    UFUNCTION()
    virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
    UFUNCTION()
    virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
    UFUNCTION()
    virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
};
```

### Реализация ключевых методов

```cpp
UMyAttributeSet::UMyAttributeSet()
{
    InitHealth(100.f);
    InitMaxHealth(100.f);
    InitMana(50.f);
    InitMaxMana(50.f);
    InitMoveSpeed(600.f);
}

void UMyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // Clamp — не даём здоровью выйти за рамки
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
    }
}

void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // Обработка мета-атрибута IncomingDamage
    if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        const float LocalDamage = GetIncomingDamage();
        SetIncomingDamage(0.f); // Сброс мета-атрибута

        if (LocalDamage > 0.f)
        {
            const float NewHealth = GetHealth() - LocalDamage;
            SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

            if (GetHealth() <= 0.f)
            {
                // Смерть — отправляем GameplayEvent
                FGameplayEventData EventData;
                EventData.Instigator = Data.EffectSpec.GetEffectContext().GetOriginalInstigator();

                UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
                if (ASC)
                {
                    FGameplayTag DeathTag = FGameplayTag::RequestGameplayTag(FName("Event.Death"));
                    ASC->HandleGameplayEvent(DeathTag, &EventData);

                    // Добавляем тег State.Dead
                    ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
                }
            }
        }
    }
}
```

### Мета-атрибуты

`IncomingDamage` — пример **мета-атрибута**. Это временный атрибут, который:
1. Принимает значение из GameplayEffect
2. Обрабатывается в `PostGameplayEffectExecute` (учитывается броня, резисты)
3. Итог применяется к `Health`
4. Сбрасывается в 0

Это даёт точку для **перехвата** урона (броня, неуязвимость, абсорб и т.д.).

### PreAttributeChange vs PostGameplayEffectExecute

| Метод | Когда | Для чего |
|---|---|---|
| `PreAttributeChange` | До изменения | Clamp значений |
| `PostGameplayEffectExecute` | После применения эффекта | Логика реакции (смерть, порог HP) |

> ⚠️ `PreAttributeChange` вызывается при **любом** изменении (и от query, и от эффекта). Не делай тут логику типа «смерть» — только clamp.

---

## 6. Gameplay Cues

**Визуальные и звуковые эффекты**, полностью отделённые от логики. Это ключевая архитектурная идея — логика не знает про VFX, VFX не знает про логику.

### Как работают

```
GameplayEffect (урон) → имеет тег GameplayCue.Damage.Physical
    │
    ▼
GameplayCue Manager → ищет обработчик по тегу
    │
    ▼
GameplayCue_DamagePhysical → спавнит Niagara, играет звук
```

### Типы Cue

| Тип | Когда вызывается | Пример |
|---|---|---|
| `OnExecute` | Один раз (Instant эффект) | Звук удара, искры |
| `OnActive` / `OnRemove` | При начале и конце Duration/Infinite эффекта | Огонь на персонаже (начал гореть / потух) |
| `WhileActive` | Каждый тик пока эффект активен | Частицы яда |

### Создание через Actor

1. ПКМ в Content Browser → **Blueprint Class** → **GameplayCueNotify_Actor**
2. Назвать `GC_Explosion`
3. Установить **Gameplay Cue Tag**: `GameplayCue.Ability.Explode`
4. Переопределить:
   - `OnExecute` — спавнить Niagara, Sound
   - `OnActive` — запустить looping эффект
   - `OnRemove` — остановить эффект

### Создание через Static (без актора — легче)

1. **GameplayCueNotify_Static** — не спавнит актор, легковесный
2. Хорош для одноразовых эффектов (звук удара, мелкие искры)

### Вызов вручную (из способности)

```cpp
// Из GameplayAbility
FGameplayCueParameters CueParams;
CueParams.Location = ExplosionLocation;
CueParams.Normal = FVector::UpVector;

// Одноразовый
K2_ExecuteGameplayCue(
    FGameplayTag::RequestGameplayTag(FName("GameplayCue.Ability.Explode")),
    CueParams
);

// Длительный (включить)
K2_AddGameplayCue(
    FGameplayTag::RequestGameplayTag(FName("GameplayCue.Debuff.OnFire")),
    CueParams
);

// Длительный (выключить)
K2_RemoveGameplayCue(
    FGameplayTag::RequestGameplayTag(FName("GameplayCue.Debuff.OnFire"))
);
```

---

## 7. Gameplay Tasks

**Базовый класс для асинхронных задач** в GAS. `UGameplayTask` — корневой класс. Управляется через `GameplayTasksComponent` (встроен в ASC).

Используется редко напрямую — обычно через **Ability Tasks**.

---

## 8. Ability Tasks

**Асинхронные действия внутри способности**. Без них способность — синхронная функция (вызвал → сделал → завершил). С ними можно:

- Ждать окончания монтажа
- Ждать нажатия кнопки
- Ждать подтверждения от сервера
- Ждать GameplayEvent
- Двигать персонажа плавно

### Встроенные AbilityTasks

| Task | Описание |
|---|---|
| `UAbilityTask_PlayMontageAndWait` | Играть монтаж, ждать завершения |
| `UAbilityTask_WaitGameplayEvent` | Ждать GameplayEvent по тегу |
| `UAbilityTask_WaitInputPress` | Ждать нажатия кнопки |
| `UAbilityTask_WaitInputRelease` | Ждать отпускания кнопки |
| `UAbilityTask_WaitTargetData` | Ждать данных о цели |
| `UAbilityTask_WaitDelay` | Просто ждать N секунд |
| `UAbilityTask_WaitGameplayEffectRemoved` | Ждать окончания эффекта |
| `UAbilityTask_WaitAbilityEnd` | Ждать завершения другой способности |
| `UAbilityTask_MoveToLocation` | Плавно двигать к точке |
| `UAbilityTask_ApplyRootMotionConstantForce` | Применить силу (для рывков) |
| `UAbilityTask_WaitGameplayTagAdded` | Ждать появления тега на акторе |
| `UAbilityTask_WaitGameplayTagRemoved` | Ждать удаления тега |
| `UAbilityTask_WaitConfirmCancel` | Ждать подтверждения или отмены |

### Пример: Взрыв с анимацией

```cpp
void UGA_EnemyExplode::ActivateAbility(...)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // Играем монтаж подготовки к взрыву
    UAbilityTask_PlayMontageAndWait* MontageTask =
        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,                   // Owning Ability
            NAME_None,              // Task Instance Name
            ExplodeMontage,         // Montage
            1.f                     // Rate
        );

    // Подписываемся на события
    MontageTask->OnCompleted.AddDynamic(this, &UGA_EnemyExplode::OnMontageCompleted);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_EnemyExplode::OnMontageCancelled);
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_EnemyExplode::OnMontageCancelled);

    // Запускаем
    MontageTask->ReadyForActivation();
}

void UGA_EnemyExplode::OnMontageCompleted()
{
    // Монтаж закончен — теперь взрываемся
    PerformExplosion();
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_EnemyExplode::OnMontageCancelled()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
```

### Создание своего AbilityTask

```cpp
UCLASS()
class UAbilityTask_WaitCloseToTarget : public UAbilityTask
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetReached);

    UPROPERTY(BlueprintAssignable)
    FOnTargetReached OnReached;

    // Фабричный метод (паттерн GAS)
    UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
    static UAbilityTask_WaitCloseToTarget* CreateTask(
        UGameplayAbility* OwningAbility,
        AActor* Target,
        float AcceptanceRadius);

    virtual void TickTask(float DeltaTime) override;

private:
    TWeakObjectPtr<AActor> TargetActor;
    float Radius;
};
```

---

## 9. Gameplay Events

**Событийная система** внутри GAS. Позволяет одному акторе послать «событие» другому, и это может **автоматически активировать способность**.

### Отправка события

```cpp
FGameplayEventData EventData;
EventData.Instigator = AttackerActor;
EventData.Target = VictimActor;
EventData.EventMagnitude = 50.f;  // Произвольные данные
EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Hit.Melee"));

// Послать событие актору — если у него есть способность с таким триггером, она активируется
UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
    VictimActor,
    EventData.EventTag,
    EventData
);
```

### Способность, активируемая событием

```cpp
UGA_OnDeath::UGA_OnDeath()
{
    // Вместо ручной активации — триггер по событию
    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(FName("Event.Death"));
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}
```

Теперь когда кто-то пошлёт `Event.Death` этому актору — способность `GA_OnDeath` активируется **автоматически**.

---

## 10. Target Data и Targeting

Система для **выбора цели** способности.

### Простой вариант (без TargetData)

Для одиночной игры / AI — достаточно обычных Trace/Overlap прямо в `ActivateAbility`.

### Полный вариант (с TargetData)

Нужен для мультиплеера — клиент выбирает цель, сервер подтверждает.

```cpp
// Создание Target Actor (показывает декал на земле, линию прицела и т.д.)
UCLASS()
class ATargetActor_GroundCircle : public AGameplayAbilityTargetActor
{
    // Показывает круг на земле
    // Игрок подтверждает — данные о позиции отправляются серверу
};
```

Для одиночной FPS-игры с AI-врагами — **TargetData не нужен**, можно обойтись Trace/Overlap внутри способности.

---

## Настройка проекта — подключение GAS

### 1. Build.cs — добавить модули

```csharp
// В FPSGame.Build.cs
PublicDependencyModuleNames.AddRange(new string[]
{
    "Core",
    "CoreUObject",
    "Engine",
    "InputCore",
    "GameplayAbilities",     // ← Основной модуль GAS
    "GameplayTags",          // ← Система тегов
    "GameplayTasks"          // ← Система задач
});
```

### 2. uproject — включить плагин (если не включён)

```json
{
    "Plugins": [
        {
            "Name": "GameplayAbilities",
            "Enabled": true
        }
    ]
}
```

### 3. Пересобрать проект

---

## Шаг за шагом: первая способность

### Цель: Враг-зомби взрывается при смерти

**Шаг 1:** Создать `UMyAttributeSet` (Health, MaxHealth) — см. раздел выше

**Шаг 2:** Добавить ASC на врага
```cpp
// AEnemyCharacter.h
UCLASS()
class AEnemyCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()
public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }

    UPROPERTY(VisibleAnywhere)
    UAbilitySystemComponent* ASC;

    UPROPERTY()
    UMyAttributeSet* Attributes;

    UPROPERTY(EditDefaultsOnly, Category = "Abilities")
    TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
};
```

**Шаг 3:** Инициализация
```cpp
AEnemyCharacter::AEnemyCharacter()
{
    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    Attributes = CreateDefaultSubobject<UMyAttributeSet>(TEXT("Attributes"));
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Инициализировать атрибуты через GE (опционально — можно из DataTable)
    if (ASC)
    {
        ASC->InitAbilityActorInfo(this, this);

        for (auto& AbilityClass : DefaultAbilities)
        {
            ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
        }
    }
}
```

**Шаг 4:** Создать `UGA_Explode` (GameplayAbility) — см. примеры выше

**Шаг 5:** Создать `GE_ExplosionDamage` (Blueprint GameplayEffect)
```
Duration Policy: Instant
Modifiers:
  Attribute: MyAttributeSet.Health
  Modifier Op: Add
  Magnitude: -100
```

**Шаг 6:** Триггер — активация при смерти
```cpp
// В PostGameplayEffectExecute, когда HP <= 0:
ASC->HandleGameplayEvent(TAG_Event_Death, &EventData);

// А GA_Explode настроен на триггер Event.Death
```

**Шаг 7:** В Blueprint врага — добавить `GA_Explode` в массив `DefaultAbilities`

---

## Практический пример: FPS способности врагов

### Набор способностей

```
Зомби-бегун:
  GA_MeleeAttack    — удар когтями
  GA_LeapAttack     — прыжок на игрока

Зомби-бомба:
  GA_SelfExplode    — взрыв при смерти или при контакте

Зомби-плевун:
  GA_PoisonSpit     — плевок (проджектайл + DOT)

Зомби-крикун:
  GA_Scream         — крик (AoE замедление)
```

### GA_PoisonSpit — Ядовитый плевок

```cpp
UCLASS()
class UGA_PoisonSpit : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_PoisonSpit()
    {
        InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
        AbilityTags.AddTag(TAG("Ability.Enemy.PoisonSpit"));
        ActivationBlockedTags.AddTag(TAG("State.Dead"));
        ActivationBlockedTags.AddTag(TAG("State.Stunned"));
    }

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AActor> ProjectileClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> PoisonDOTEffect; // Duration, -10 HP/сек, 5 сек

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> CooldownEffect;

    UPROPERTY(EditDefaultsOnly)
    UAnimMontage* SpitMontage;

    virtual void ActivateAbility(...) override
    {
        if (!CommitAbility(...)) { EndAbility(...); return; }

        // 1. Монтаж плевка
        UAbilityTask_PlayMontageAndWait* Task =
            UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
                this, NAME_None, SpitMontage, 1.f);
        Task->OnCompleted.AddDynamic(this, &UGA_PoisonSpit::OnSpitComplete);
        Task->ReadyForActivation();
    }

    void OnSpitComplete()
    {
        // 2. Спавн проджектайла
        AActor* Avatar = GetAvatarActorFromActorInfo();
        FVector SpawnLoc = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 100.f;
        FRotator SpawnRot = Avatar->GetActorRotation();

        AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLoc, SpawnRot);

        // Проджектайл при попадании применит PoisonDOTEffect к цели
        // (логика в самом проджектайле)

        EndAbility(...);
    }
};
```

### GE_PoisonDOT (Blueprint)

```
Duration Policy: Has Duration
Duration Magnitude: 5.0 (секунд)
Period: 1.0 (каждую секунду)

Modifiers:
  Attribute: MyAttributeSet.Health
  Modifier Op: Add
  Magnitude: -10

Tags:
  Granted Tags: Debuff.Poison    ← Тег на акторе пока эффект активен
  Asset Tag: Damage.Poison

Stacking:
  Type: Aggregate by Source
  Limit: 3                       ← Максимум 3 стака
  Duration Refresh: Refresh
```

### GA_Scream — AoE замедление

```
GE_ScreamSlow (Blueprint GameplayEffect):
  Duration: 3.0 сек
  Modifiers:
    Attribute: MyAttributeSet.MoveSpeed
    Modifier Op: Multiply
    Magnitude: 0.5 (скорость × 0.5 = замедление в 2 раза)
  Granted Tags: Debuff.Slow
  GameplayCue: GameplayCue.Debuff.Slow
```

---

## Attribute Set — подробный разбор

### FGameplayAttributeData

Каждый атрибут — это `FGameplayAttributeData`, содержащий:
- **BaseValue** — базовое значение (без временных модификаторов)
- **CurrentValue** — текущее значение (с учётом Duration/Infinite эффектов)

```
Пример: базовая скорость = 600
  + Баф скорости (+200, Duration) = CurrentValue: 800
  Баф закончился → CurrentValue вернулось к 600

Instant эффект меняет BaseValue напрямую:
  HP BaseValue: 100
  + Урон (-30, Instant) → BaseValue: 70, CurrentValue: 70
```

### Инициализация через DataTable

Вместо хардкода в конструкторе:

```cpp
// В BeginPlay
if (ASC && InitDataTable)
{
    // Загружает значения атрибутов из DataTable
    const UAttributeSet* Set = ASC->GetOrCreateAttributeSubobject(UMyAttributeSet::StaticClass());
    ASC->InitStats(UMyAttributeSet::StaticClass(), InitDataTable);
}
```

DataTable (Row: `AttributeMetaData`):

| RowName | Attribute | BaseValue |
|---|---|---|
| Health | MyAttributeSet.Health | 100 |
| MaxHealth | MyAttributeSet.MaxHealth | 100 |
| MoveSpeed | MyAttributeSet.MoveSpeed | 600 |

### Несколько AttributeSet на одном акторе

Можно иметь несколько:

```cpp
UPROPERTY()
UHealthAttributeSet* HealthAttributes;

UPROPERTY()
UCombatAttributeSet* CombatAttributes;

UPROPERTY()
UMovementAttributeSet* MovementAttributes;
```

ASC автоматически найдёт все AttributeSet, созданные как subobject.

### Слушатель изменений атрибута

```cpp
// Подписка на изменение Health (для UI)
AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
    UMyAttributeSet::GetHealthAttribute()
).AddUObject(this, &AMyCharacter::OnHealthChanged);

void AMyCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    float OldValue = Data.OldValue;
    float NewValue = Data.NewValue;

    // Обновить UI
    UpdateHealthBar(NewValue / GetMaxHealth());
}
```

---

## Gameplay Effects — подробный разбор

### Полная анатомия GameplayEffect

```
GE_FireDamageOverTime
│
├── Duration Policy: Has Duration
│   └── Duration: 5.0 sec
│
├── Period: 1.0 sec (тик каждую секунду)
│
├── Modifiers (массив):
│   ├── [0] Attribute: Health
│   │       Op: Add
│   │       Magnitude Type: Scalable Float
│   │       Value: -15
│   │
│   └── [1] Attribute: MoveSpeed
│           Op: Multiply
│           Magnitude Type: Scalable Float
│           Value: 0.8 (замедление на 20%)
│
├── Executions (массив):
│   └── [0] Execution Class: UMyDamageExecution (продвинутый расчёт)
│
├── Conditional Effects:
│   └── Если цель имеет тег "Debuff.Wet" → дополнительно применить GE_ElectricShock
│
├── Stacking:
│   ├── Type: Aggregate by Source
│   ├── Limit: 3
│   └── Duration Refresh: RefreshOnSuccessfulApplication
│
├── Tags:
│   ├── Asset Tags: Damage.Fire, DOT
│   ├── Granted Tags: Debuff.OnFire     (даёт тег пока эффект активен)
│   ├── Remove Effects With Tags: Debuff.Frozen  (огонь снимает заморозку)
│   ├── Application Required Tags: (пусто)
│   └── Application Blocked Tags: State.Invulnerable  (не применяется к неуязвимым)
│
├── Gameplay Cues:
│   └── GameplayCue.Debuff.OnFire      (огонь на модели)
│
└── Immunity:
    └── Если на цели тег "State.FireImmune" → эффект не применяется
```

### Magnitude Types — откуда берётся число

| Тип | Описание | Пример |
|---|---|---|
| `Scalable Float` | Фиксированное число (можно с CurveTable) | Урон = 50 |
| `Attribute Based` | На основе атрибута кастера или цели | Урон = Attack * 1.5 |
| `Custom Calculation Class` | Своя формула (C++ класс) | Урон = (Attack - Armor) * CritMultiplier |
| `Set By Caller` | Задаётся при создании SpecHandle | Динамический урон из способности |

### Set By Caller — динамические значения

```cpp
// В GameplayAbility:
FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffectClass);

// Устанавливаем значение по тегу
FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
Spec.Data->SetSetByCallerMagnitude(DamageTag, CalculatedDamage);

ApplyGameplayEffectSpecToTarget(..., Spec, TargetASC);
```

В Blueprint GameplayEffect:
```
Modifier Magnitude Type: Set By Caller
Set By Caller Tag: Data.Damage
```

### Execution Calculation Class — кастомная формула

Для сложных формул (урон с учётом брони, криты, стихии):

```cpp
UCLASS()
class UDamageExecutionCalc : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UDamageExecutionCalc();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
```

```cpp
UDamageExecutionCalc::UDamageExecutionCalc()
{
    // Объявляем, какие атрибуты нужны для расчёта
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UMyAttributeSet::GetHealthAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        false
    ));
}

void UDamageExecutionCalc::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    // Получаем SetByCaller урон
    float BaseDamage = Spec.GetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, 0.f);

    // Захватываем атрибуты цели (например, броню)
    float TargetArmor = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        /* CapturedAttribute */, FAggregatorEvaluateParameters(), TargetArmor);

    // Формула
    float FinalDamage = FMath::Max(BaseDamage - TargetArmor, 0.f);

    // Выходной модификатор — применить к IncomingDamage (мета-атрибут)
    OutExecutionOutput.AddOutputModifier(
        FGameplayModifierEvaluatedData(
            UMyAttributeSet::GetIncomingDamageAttribute(),
            EGameplayModOp::Additive,
            FinalDamage
        )
    );
}
```

### Кулдаун через GameplayEffect

```cpp
// В GameplayAbility
virtual UGameplayEffect* GetCooldownGameplayEffect() const override
{
    return CooldownEffect.GetDefaultObject();
}

// CooldownEffect — это Blueprint GameplayEffect:
// Duration: 5 сек
// Granted Tag: Cooldown.Ability.Explode
// (Больше ничего — просто длительность + тег)
```

```cpp
// В конструкторе способности
CooldownGameplayEffectClass = UGE_ExplodeCooldown::StaticClass();
// Или задать в Blueprint
```

### Стоимость через GameplayEffect

```cpp
virtual UGameplayEffect* GetCostGameplayEffect() const override
{
    return CostEffect.GetDefaultObject();
}

// CostEffect (Blueprint):
// Duration: Instant
// Modifier: Mana, Add, -30 (тратит 30 маны)
```

---

## Gameplay Tags — подробный разбор

### Иерархия и наследование

```
Ability
├── Ability.Enemy
│   ├── Ability.Enemy.Explode
│   ├── Ability.Enemy.PoisonSpit
│   └── Ability.Enemy.Scream
└── Ability.Player
    ├── Ability.Player.Shoot
    └── Ability.Player.Dash
```

Если проверяешь тег `Ability.Enemy` — он **матчит** все подтеги:
```cpp
FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag(FName("Ability.Enemy"));
FGameplayTag ExplodeTag = FGameplayTag::RequestGameplayTag(FName("Ability.Enemy.Explode"));

// true — Explode "является" дочерним тегом Enemy
ExplodeTag.MatchesTag(EnemyTag); // true

// false — Enemy не является Explode
EnemyTag.MatchesTag(ExplodeTag); // false

// Точное совпадение
ExplodeTag.MatchesTagExact(EnemyTag); // false
```

### Контейнеры тегов

```cpp
FGameplayTagContainer Container;
Container.AddTag(TAG("State.Stunned"));
Container.AddTag(TAG("Debuff.Poison"));

// Есть ли ХОТЯ БЫ ОДИН из тегов?
FGameplayTagContainer Query;
Query.AddTag(TAG("State.Stunned"));
Query.AddTag(TAG("State.Dead"));

Container.HasAny(Query);  // true — есть Stunned
Container.HasAll(Query);  // false — нет Dead
```

### Добавление/Удаление тегов на акторе через ASC

```cpp
// Добавить (loose — не привязан к эффекту)
ASC->AddLooseGameplayTag(TAG("State.Enraged"));

// Убрать
ASC->RemoveLooseGameplayTag(TAG("State.Enraged"));

// Проверить
ASC->HasMatchingGameplayTag(TAG("State.Enraged")); // true/false

// Подписаться на изменение тега
ASC->RegisterGameplayTagEvent(
    TAG("State.Dead"),
    EGameplayTagEventType::NewOrRemoved
).AddUObject(this, &AMyCharacter::OnDeadTagChanged);

void AMyCharacter::OnDeadTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount > 0)
    {
        // Персонаж мёртв
        Die();
    }
}
```

---

## Gameplay Cues — визуальные и звуковые эффекты

### Полный пример: Cue для взрыва

```cpp
// GameplayCueNotify_Actor Blueprint: GC_Explosion
// Tag: GameplayCue.Ability.Explode

// В Event Graph:

// OnExecute:
//   1. Spawn Niagara System (ExplosionVFX) at Location
//   2. Play Sound at Location (ExplosionSFX)
//   3. Camera Shake (если близко к игроку)
```

### Cue Parameters — что можно передать

```cpp
FGameplayCueParameters Params;
Params.Location = ExplosionLocation;         // Где
Params.Normal = HitNormal;                   // Направление
Params.PhysicalMaterial = HitPhysMat;        // Материал (для разных звуков)
Params.EffectContext = EffectContextHandle;   // Полный контекст эффекта
Params.RawMagnitude = DamageAmount;           // Число (для масштабирования VFX)
Params.SourceObject = InstigatorActor;        // Кто вызвал
Params.TargetAttachComponent = MeshComp;      // К чему прикрепить
```

### Соглашение об именовании

**Тег Cue ОБЯЗАТЕЛЬНО должен начинаться с `GameplayCue.`**

```
GameplayCue.Ability.Explode         ✅
GameplayCue.Damage.Physical         ✅
GameplayCue.Debuff.OnFire           ✅
Ability.Explode                     ❌ (не найдёт)
```

---

## Ability Tasks — асинхронные действия внутри способностей

### Паттерн использования

```cpp
void UMyAbility::ActivateAbility(...)
{
    // 1. Создать Task через статический метод
    UAbilityTask_WaitGameplayEvent* WaitTask =
        UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
            this,                                                     // Ability
            FGameplayTag::RequestGameplayTag(FName("Event.Hit")),     // Тег
            nullptr,                                                  // Opt. Filter Actor
            false,                                                    // Only Trigger Once
            false                                                     // Only Match Exact
        );

    // 2. Подписаться на делегат
    WaitTask->EventReceived.AddDynamic(this, &UMyAbility::OnHitEventReceived);

    // 3. Активировать Task
    WaitTask->ReadyForActivation();

    // ВАЖНО: код после ReadyForActivation продолжает выполняться!
    // Task работает асинхронно
}

void UMyAbility::OnHitEventReceived(FGameplayEventData Payload)
{
    // Среагировали на событие
    ApplyDamage(Payload.Target);
    EndAbility(...);
}
```

### В Blueprint

AbilityTasks — это **латентные ноды** (с выходными execution pins). Выглядят как `Delay`, `MoveTo` и т.д.

```
[Play Montage And Wait] ───┬── On Completed ──→ [Apply Damage] → [End Ability]
                           ├── On Interrupted ──→ [End Ability]
                           └── On Cancelled ────→ [End Ability]
```

---

## Networking / Мультиплеер

### Prediction (предсказание)

GAS поддерживает **клиентское предсказание**: клиент начинает выполнять способность **сразу**, не дожидаясь сервера. Если сервер отклонит — откатывается.

```
Клиент: Нажал кнопку → TryActivateAbility → Начал анимацию
    │
    └──(отправка на сервер)──→ Сервер: Проверяет → Подтверждает / Отклоняет
                                │
                                └── Если отклонил → Клиент откатывает
```

### Что предсказывается автоматически

- Активация способности
- Игра монтажа
- Применение GameplayEffect (кулдаун, стоимость)
- Добавление/удаление GameplayTags

### Что НЕ предсказывается

- Изменение атрибутов у других игроков
- Спавн акторов

### Настройка

```cpp
// В конструкторе GameplayAbility
NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
// Или ServerInitiated для AI
```

### Для одиночной игры

Если у тебя **синглплеер** — можно не думать о сетевой части. Используй `ServerOnly` или `LocalOnly` и не реплицируй ничего.

---

## Архитектурные паттерны и лучшие практики

### 1. GameplayEffect — ТОЛЬКО в Blueprint

```
✅ GE_ExplosionDamage (Blueprint)     — дизайнер меняет урон без компиляции
❌ UGE_ExplosionDamage (C++)          — неудобно, каждое изменение = пересборка
```

**Исключение:** если нужна динамическая генерация эффекта в рантайме.

### 2. GameplayAbility — C++ база + Blueprint конкретика

```
UGA_RangedAttackBase (C++)            — общая логика стрельбы
  └── GA_PoisonSpit (Blueprint)       — конкретные настройки (монтаж, эффект, проджектайл)
  └── GA_FireBreath (Blueprint)       — другие настройки
```

### 3. Мета-атрибуты для урона

Не применяй урон напрямую к Health. Используй промежуточный `IncomingDamage`:

```
GE_Damage → IncomingDamage (мета)
    │
    ▼
PostGameplayEffectExecute:
    FinalDamage = IncomingDamage - Armor
    Health -= FinalDamage
```

### 4. Теги — планируй заранее

Создай чёткую иерархию **до** начала реализации:

```
State.          — состояния актора
Ability.        — способности
Cooldown.       — кулдауны
Damage.         — типы урона
Debuff.         — дебафы
Buff.           — бафы
Event.          — события
GameplayCue.    — визуальные эффекты
Data.           — SetByCaller данные
```

### 5. ASC на PlayerState (мультиплеер)

Для игрока в мультиплеере:
```cpp
// PlayerState хранит ASC
class AMyPlayerState : public APlayerState, public IAbilitySystemInterface
{
    UPROPERTY()
    UAbilitySystemComponent* ASC;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return ASC; }
};

// Character ссылается на ASC из PlayerState
class AMyCharacter : public ACharacter, public IAbilitySystemInterface
{
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
    {
        AMyPlayerState* PS = GetPlayerState<AMyPlayerState>();
        return PS ? PS->GetAbilitySystemComponent() : nullptr;
    }
};
```

### 6. Не забывай EndAbility()

```cpp
// КАЖДЫЙ путь выполнения должен заканчиваться EndAbility()
void UMyAbility::ActivateAbility(...)
{
    if (!CommitAbility(...))
    {
        EndAbility(..., true, true);  // ← cancelled
        return;
    }

    // ... логика ...

    EndAbility(..., false, false);    // ← нормальное завершение
}
```

Если забудешь — способность **навсегда** останется «активной», блокируя другие.

---

## Дерево решений: нужен ли GAS?

```
У тебя больше 3-5 разных способностей?
├── НЕТ → Своя система на UObject/Component (проще)
└── ДА
    │
    Нужны ли бафы/дебафы/DOT с наложением?
    ├── НЕТ → Можно без GAS
    └── ДА
        │
        Нужен ли мультиплеер с предсказанием?
        ├── НЕТ → GAS — удобен, но можно и без него
        └── ДА → GAS — ОЧЕНЬ рекомендуется (предсказание из коробки)
            │
            Готов потратить 1-2 недели на изучение?
            ├── ДА → Используй GAS ✅
            └── НЕТ → Своя система, мультиплеер позже
```

### Итого для твоего проекта (FPS с зомби)

- **Если одиночная игра** и способностей мало (взрыв, плевок, крик) → своя система на UObject проще и быстрее
- **Если планируется мультиплеер** или способностей будет много (10+, с бафами, стаками, взаимодействиями между ними) → GAS окупится
- **Если хочешь изучить GAS** для карьеры Gameplay Programmer → однозначно стоит, это стандарт индустрии

---

## Частые ошибки

### ❌ Забыл IAbilitySystemInterface

```cpp
// GAS не может найти ASC на акторе → ничего не работает
class AMyCharacter : public ACharacter // ← Нет IAbilitySystemInterface!
```

### ❌ Не вызвал InitAbilityActorInfo

```cpp
// ASC не знает своего владельца → крашит при активации
void AMyCharacter::BeginPlay()
{
    // ОБЯЗАТЕЛЬНО:
    ASC->InitAbilityActorInfo(this, this);
}
```

### ❌ GameplayEffect в C++ вместо Blueprint

Менять урон = перекомпиляция проекта. **Создавай GE в Blueprint.**

### ❌ Забыл EndAbility

Способность «висит» активной навсегда. Все другие способности заблокированы.

### ❌ Прямое изменение атрибутов

```cpp
// НЕПРАВИЛЬНО — обходит GAS, эффекты не знают об изменении
Attributes->SetHealth(50.f);

// ПРАВИЛЬНО — через GameplayEffect
ApplyGameplayEffectToSelf(HealEffect, ...);
```

### ❌ NonInstanced с хранением состояния

```cpp
// КРАШ — NonInstanced = один объект на всех, нет своих переменных
InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
float MyTimer; // ← Все акторы пишут в одну переменную
```

### ❌ Тег Cue без префикса GameplayCue

```
GameplayCue.Explosion  ✅ — найдёт
Explosion              ❌ — не найдёт
```

### ❌ Не добавил модули в Build.cs

```
Unresolved external symbol... UAbilitySystemComponent...
→ Добавь GameplayAbilities, GameplayTags, GameplayTasks в Build.cs
```

---

## Шпаргалка — основные классы

| Класс | Назначение |
|---|---|
| `UAbilitySystemComponent` | Центральный компонент. Хранит всё |
| `UGameplayAbility` | Базовый класс способности |
| `UGameplayEffect` | Эффект (урон, баф, DOT, кулдаун) |
| `UAttributeSet` | Набор атрибутов (HP, Mana, Speed) |
| `FGameplayTag` | Один тег |
| `FGameplayTagContainer` | Контейнер тегов |
| `FGameplayAbilitySpec` | Спецификация выданной способности |
| `FGameplayEffectSpec` | Спецификация применяемого эффекта |
| `FGameplayEffectSpecHandle` | Хэндл на спецификацию эффекта |
| `FGameplayEffectContextHandle` | Контекст эффекта (кто, откуда) |
| `UAbilityTask` | Базовый класс асинхронных задач |
| `UAbilityTask_PlayMontageAndWait` | Играть монтаж и ждать |
| `UAbilityTask_WaitGameplayEvent` | Ждать событие по тегу |
| `UAbilityTask_WaitGameplayTagAdded` | Ждать появления тега |
| `UGameplayEffectExecutionCalculation` | Кастомная формула расчёта |
| `UGameplayModMagnitudeCalculation` | Кастомный расчёт магнитуды |
| `AGameplayCueNotify_Actor` | VFX/SFX (актор, для длительных) |
| `UGameplayCueNotify_Static` | VFX/SFX (без актора, для мгновенных) |
| `IAbilitySystemInterface` | Интерфейс — обязателен на акторе |
| `UAbilitySystemBlueprintLibrary` | Утилиты для Blueprint / C++ |
| `FGameplayAbilityTargetDataHandle` | Данные о цели |
| `AGameplayAbilityTargetActor` | Визуальный выбор цели |

---

## Полезные функции ASC

```cpp
// Выдать способность
ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, Level, InputID, SourceObject));

// Забрать способность
ASC->ClearAbility(AbilitySpecHandle);

// Активировать
ASC->TryActivateAbilityByClass(AbilityClass);
ASC->TryActivateAbilitiesByTag(TagContainer);

// Отменить
ASC->CancelAbilityByClass(AbilityClass);
ASC->CancelAbilities(&TagContainer); // По тегу
ASC->CancelAllAbilities();

// Применить эффект
FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(EffectClass, Level, ContextHandle);
ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
ASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);

// Удалить эффект
ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
ASC->RemoveActiveEffectsWithTags(TagContainer);

// Теги
ASC->AddLooseGameplayTag(Tag);
ASC->RemoveLooseGameplayTag(Tag);
ASC->HasMatchingGameplayTag(Tag);
ASC->GetOwnedGameplayTags(OutTagContainer);

// Атрибуты
ASC->GetNumericAttribute(UMyAttributeSet::GetHealthAttribute());
ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(...);

// Инициализация
ASC->InitAbilityActorInfo(OwnerActor, AvatarActor);
```

---

## Поток данных — как всё связано

```
                    ┌──────────────────────────────┐
                    │         AI / Input            │
                    │   (решает КОГДА активировать) │
                    └──────────────┬───────────────┘
                                   │
                                   ▼
                    ┌──────────────────────────────┐
                    │   TryActivateAbilityByTag()  │
                    └──────────────┬───────────────┘
                                   │
                                   ▼
                    ┌──────────────────────────────┐
                    │     CanActivateAbility()     │
                    │  • Проверка тегов            │
                    │  • Проверка кулдауна         │
                    │  • Проверка стоимости         │
                    └──────────────┬───────────────┘
                                   │ (если true)
                                   ▼
                    ┌──────────────────────────────┐
                    │      ActivateAbility()       │
                    │  • CommitAbility()            │
                    │    ├── ApplyCost (GE)         │
                    │    └── ApplyCooldown (GE)     │
                    │  • AbilityTasks               │
                    │    ├── PlayMontage             │
                    │    ├── WaitEvent               │
                    │    └── ...                     │
                    └──────────────┬───────────────┘
                                   │
                                   ▼
                    ┌──────────────────────────────┐
                    │   ApplyGameplayEffect()      │
                    │  → Modifiers / Executions    │
                    │  → Меняет атрибуты цели      │
                    │  → Запускает GameplayCues     │
                    │  → Добавляет теги             │
                    └──────────────┬───────────────┘
                                   │
                          ┌────────┴────────┐
                          ▼                 ▼
               ┌──────────────┐   ┌──────────────────┐
               │ AttributeSet │   │  GameplayCue     │
               │ PostExecute  │   │  (VFX / SFX)     │
               │ → Смерть?    │   │  → Niagara       │
               │ → UI Update  │   │  → Sound         │
               └──────────────┘   └──────────────────┘
                          │
                          ▼
                    ┌──────────────────────────────┐
                    │        EndAbility()          │
                    └──────────────────────────────┘
```

