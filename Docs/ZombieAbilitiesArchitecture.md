# Архитектура системы способностей зомби (Zombie Abilities)

## Философия

Та же, что и в оружейной системе:
- **UObject** — чистая логика способности (урон, радиус, кулдаун, тик)
- **UDataAsset** — настройки (дизайнер крутит ползунки, код не трогает)
- **UActorComponent** — менеджер на акторе зомби, управляет массивом способностей
- **State Tree** решает **КОГДА** использовать, компонент решает **МОЖНО ЛИ**, UObject делает **ЧТО**

Логика способности **не знает** про AI. AI **не знает** про внутренности способности.

---

## Диаграмма классов

```
UDataAsset
 └── UFP_AbilityDataAsset              ← конфиг: урон, радиус, кулдаун, монтаж, VFX, SFX

UObject
 └── UFP_AbilityBase                   ← базовая логика способности
      ├── UFP_Ability_MeleeAttack      ← удар когтями (сфера-трейс перед собой)
      ├── UFP_Ability_Explode          ← самоподрыв (AoE overlap)
      ├── UFP_Ability_PoisonSpit       ← ядовитый плевок (спавн проджектайла + DOT)
      └── UFP_Ability_Scream           ← крик (AoE дебаф замедление)

UActorComponent
 └── UFP_AbilityComponent              ← менеджер на зомби, хранит массив UFP_AbilityBase*

UDataAsset (уже существует)
 └── UFP_ZombieConfig                  ← добавить: TArray<UFP_AbilityDataAsset*> Abilities
```

---

## Связь с существующим кодом

```
AFP_BaseZombie (уже есть)
 ├── UStateTreeComponent          (уже есть)    ← AI решает когда
 ├── UFP_HealthComponent          (наследуется)  ← HP зомби
 ├── UFP_ZombieConfig             (уже есть)     ← добавить массив способностей
 └── UFP_AbilityComponent         (НОВЫЙ)        ← управляет способностями
      └── TArray<UFP_AbilityBase*>               ← живые экземпляры способностей
           ├── UFP_Ability_MeleeAttack [0]
           └── UFP_Ability_Explode     [1]
```

---

## Файловая структура (куда что класть)

```
Source/FPSGame/
├── Public/
│   ├── DataAsset/
│   │   └── Zombie/
│   │       ├── FP_ZombieConfig.h           ← (уже есть, добавить TArray Abilities)
│   │       └── FP_AbilityDataAsset.h       ← НОВЫЙ
│   │
│   ├── Component/
│   │   └── Zombie/
│   │       └── FP_AbilityComponent.h       ← НОВЫЙ
│   │
│   └── Ability/
│       ├── FP_AbilityBase.h                ← НОВЫЙ (базовый UObject)
│       ├── FP_Ability_MeleeAttack.h        ← НОВЫЙ
│       ├── FP_Ability_Explode.h            ← НОВЫЙ
│       ├── FP_Ability_PoisonSpit.h         ← НОВЫЙ
│       └── FP_Ability_Scream.h             ← НОВЫЙ
│
└── Private/
    ├── Component/
    │   └── Zombie/
    │       └── FP_AbilityComponent.cpp
    │
    └── Ability/
        ├── FP_AbilityBase.cpp
        ├── FP_Ability_MeleeAttack.cpp
        ├── FP_Ability_Explode.cpp
        ├── FP_Ability_PoisonSpit.cpp
        └── FP_Ability_Scream.cpp
```

---

## Класс 1: `UFP_AbilityDataAsset` (UDataAsset)

**Ответственность:** Хранит настройки одной способности. Создаётся в Content Browser, назначается в ZombieConfig.

### Поля

| Поле | Тип | Описание |
|------|-----|----------|
| `AbilityName` | `FName` | Имя для дебага / UI |
| `AbilityClass` | `TSubclassOf<UFP_AbilityBase>` | Какой класс логики создать |
| `Cooldown` | `float` | Перезарядка (секунды) |
| `Damage` | `float` | Урон |
| `Range` | `float` | Дальность / радиус действия |
| `ActivationDistance` | `float` | На каком расстоянии от цели можно активировать |
| `Duration` | `float` | Длительность (для DOT, дебафов). 0 = мгновенный |
| `Period` | `float` | Интервал тика (для DOT). 0 = нет тика |
| `Priority` | `int32` | Приоритет выбора AI (чем выше — тем приоритетнее) |
| `ActivationMontage` | `UAnimMontage*` | Монтаж при активации |
| `ActivationSound` | `USoundBase*` | Звук |
| `VFX` | `UNiagaraSystem*` | Визуальный эффект (опционально) |

### Пример ассета в редакторе

```
DA_Ability_Explode:
  AbilityName: "SelfExplode"
  AbilityClass: UFP_Ability_Explode
  Cooldown: 0        (одноразовая)
  Damage: 80
  Range: 500         (радиус взрыва)
  ActivationDistance: 200  (дойти до игрока на 200 юнитов)
  Duration: 0
  Period: 0
  Priority: 10       (высший приоритет)
  ActivationMontage: AM_ZombieExplode
  ActivationSound: SFX_Explosion
  VFX: NS_Explosion
```

---

## Класс 2: `UFP_AbilityBase` (UObject)

**Ответственность:** Базовая логика способности. Все конкретные способности наследуют от него.

### Поля

| Поле | Тип | Описание |
|------|-----|----------|
| `AbilityData` | `UFP_AbilityDataAsset*` | Ссылка на конфиг |
| `OwnerActor` | `TWeakObjectPtr<AActor>` | Владелец (зомби) |
| `bIsActive` | `bool` | Сейчас выполняется? |
| `bIsOnCooldown` | `bool` | На перезарядке? |
| `CooldownTimerHandle` | `FTimerHandle` | Таймер кулдауна |

### Методы

| Метод | Описание |
|-------|----------|
| `Initialize(Data, Owner)` | Вызывается компонентом при создании. Сохраняет ссылки |
| `CanActivate() → bool` | **virtual**. Базовая проверка: не активна, не на кулдауне, владелец жив. Дочерние классы добавляют свои условия (расстояние, видимость) |
| `Activate()` | **virtual**. Запуск. Базовая логика: `bIsActive = true`, проиграть монтаж/звук. Дочерний класс: своя логика (overlap, trace, spawn) |
| `Deactivate()` | **virtual**. Принудительная отмена (владелец умер, оглушён). Останавливает монтаж, ставит `bIsActive = false` |
| `OnCompleted()` | **virtual**. Нормальное завершение. `bIsActive = false`, запускает `StartCooldown()`, бродкастит делегат |
| `StartCooldown()` | Запускает таймер по `AbilityData->Cooldown`. По окончании → `OnCooldownFinished()` |
| `OnCooldownFinished()` | `bIsOnCooldown = false` |

### Делегат

```
FOnAbilityFinished  — бродкастится в OnCompleted(). 
                      AbilityComponent подписан → уведомляет AI
```

### Жизненный цикл

```
AbilityComponent создаёт → Initialize(Data, Owner)
    │
AI решает активировать
    │
    ▼
CanActivate()
    ├── false → AI пробует другую
    └── true
         │
         ▼
    Activate()
    │  bIsActive = true
    │  Играет монтаж / звук
    │  Дочерний класс: overlap / trace / spawn projectile
    │
    ├── (нормально) → OnCompleted()
    │                    bIsActive = false
    │                    StartCooldown()
    │                    OnAbilityFinished.Broadcast()
    │
    └── (прервано: владелец умер) → Deactivate()
                                     bIsActive = false
                                     (без кулдауна)
```

---

## Класс 3: `UFP_AbilityComponent` (UActorComponent)

**Ответственность:** Живёт на акторе зомби. Создаёт, хранит и управляет массивом способностей.

### Поля

| Поле | Тип | Описание |
|------|-----|----------|
| `Abilities` | `TArray<UFP_AbilityBase*>` | Живые экземпляры способностей (UPROPERTY!) |
| `ActiveAbility` | `UFP_AbilityBase*` | Текущая активная (или nullptr) |

### Методы

| Метод | Описание |
|-------|----------|
| `InitializeAbilities(TArray<UFP_AbilityDataAsset*>)` | Для каждого DataAsset: `NewObject<UFP_AbilityBase>(this, Data->AbilityClass)`, вызвать `Initialize(Data, GetOwner())`, добавить в массив |
| `TryActivateByIndex(int32) → bool` | Проверяет `CanActivate()`, если да → `Activate()`. Возвращает успех |
| `TryActivateByClass(TSubclassOf<UFP_AbilityBase>) → bool` | Ищет в массиве по классу, активирует |
| `GetBestAbility() → UFP_AbilityBase*` | Возвращает доступную способность с наивысшим приоритетом (`CanActivate() == true`, сортировка по `AbilityData->Priority`). AI вызывает это |
| `CancelActiveAbility()` | Если есть `ActiveAbility` → `Deactivate()` |
| `DeactivateAll()` | Останавливает всё (вызывается при смерти зомби) |
| `IsAnyAbilityActive() → bool` | Есть ли сейчас активная способность |

### Инициализация (в BeginPlay зомби)

```
AFP_BaseZombie::BeginPlay()
{
    Super::BeginPlay();
    
    if (ZombieDataAsset && AbilityComponent)
    {
        AbilityComponent->InitializeAbilities(ZombieDataAsset->Abilities);
    }
}
```

### Делегат

```
FOnAbilityStateChanged  — (опционально) бродкастит когда способность началась/закончилась.
                          State Tree Task может подписаться чтобы знать когда выйти из стейта.
```

---

## Класс 4: Конкретные способности

### `UFP_Ability_MeleeAttack` — Удар когтями

**Логика в Activate():**
1. Вызвать `Super::Activate()` (монтаж, звук, `bIsActive = true`)
2. Подписаться на окончание монтажа (или использовать AnimNotify из монтажа)
3. В момент удара (середина монтажа / AnimNotify):
   - `SphereTrace` или `SphereOverlap` из позиции владельца + ForwardVector × TraceDistance
   - Радиус из `AbilityData->Range`
   - Для каждого найденного актора: `IFP_Damageable::Execute_ReceiveDamage(Actor, AbilityData->Damage)`
4. По окончании монтажа → `OnCompleted()`

**CanActivate() дополнительно:**
- Цель ближе чем `AbilityData->ActivationDistance`

---

### `UFP_Ability_Explode` — Самоподрыв

**Логика в Activate():**
1. `Super::Activate()` (монтаж подготовки к взрыву)
2. По окончании монтажа:
   - `SphereOverlapMulti` в позиции владельца, радиус = `AbilityData->Range`
   - Для каждого актора → `IFP_Damageable::Execute_ReceiveDamage(Actor, AbilityData->Damage)`
   - Спавнить VFX и SFX из AbilityData
   - Уничтожить владельца (`OwnerActor->Destroy()`)
3. `OnCompleted()` (кулдаун не нужен — зомби мёртв)

**CanActivate() дополнительно:**
- Цель ближе чем `AbilityData->ActivationDistance`
- HP владельца ниже порога (опционально — взрывается только при низком HP)

---

### `UFP_Ability_PoisonSpit` — Ядовитый плевок

**Дополнительные поля (в DataAsset или в самом классе):**

| Поле | Тип | Описание |
|------|-----|----------|
| `ProjectileClass` | `TSubclassOf<AActor>` | Класс проджектайла (отдельный Actor) |

**Логика в Activate():**
1. `Super::Activate()` (монтаж плевка)
2. В момент плевка (AnimNotify или по таймеру):
   - Спавн проджектайла в позиции владельца + ForwardVector × 100
   - Направление = к игроку (или ForwardVector)
   - Проджектайл хранит `Damage`, `Duration`, `Period` из AbilityData
3. `OnCompleted()`

**Проджектайл (отдельный AActor — `AFP_PoisonProjectile`):**
- `UProjectileMovementComponent` — летит
- `USphereComponent` — коллизия
- При попадании (`OnComponentHit`):
  - Мгновенный урон: `IFP_Damageable::Execute_ReceiveDamage(Actor, Damage)`
  - DOT: запустить таймер с `Period`, тикать `Duration / Period` раз, каждый тик → урон
  - Уничтожить себя

**CanActivate() дополнительно:**
- Цель в зоне видимости (LineTrace от глаз — нет стены между зомби и игроком)
- Цель дальше чем ActivationDistance (не плюёт в упор)

---

### `UFP_Ability_Scream` — Крик (AoE замедление)

**Логика в Activate():**
1. `Super::Activate()` (монтаж крика, звук)
2. `SphereOverlapMulti` в позиции владельца, радиус = `AbilityData->Range`
3. Для каждого найденного персонажа:
   - Получить `CharacterMovementComponent`
   - Сохранить оригинальную скорость
   - Установить `MaxWalkSpeed *= 0.5` (или значение из DataAsset)
   - Запустить таймер на `AbilityData->Duration`
   - По таймеру: вернуть оригинальную скорость
4. `OnCompleted()`

**Или через интерфейс:** Создать `IFP_Debuffable` интерфейс с методом `ApplySlowDebuff(float Multiplier, float Duration)`, чтобы логика замедления была на стороне персонажа.

---

## Изменения в существующих классах

### `UFP_ZombieConfig` — добавить массив способностей

```
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
TArray<TObjectPtr<UFP_AbilityDataAsset>> Abilities;
```

### `AFP_BaseZombie` — добавить компонент

```
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
TObjectPtr<UFP_AbilityComponent> AbilityComponent;

// В конструкторе:
AbilityComponent = CreateDefaultSubobject<UFP_AbilityComponent>(TEXT("AbilityComponent"));

// В BeginPlay (после InitStats):
if (ZombieDataAsset && AbilityComponent)
{
    AbilityComponent->InitializeAbilities(ZombieDataAsset->Abilities);
}
```

### `AFP_BaseZombie` — убрать захардкоженные атака-поля

Текущие поля `CurrentAttackTraceDistance`, `CurrentAttackRadius`, `CurrentAttackDamage` и `InitStats()` **больше не нужны** — эти данные теперь живут в `UFP_AbilityDataAsset` каждой конкретной способности. Можно удалить когда система способностей заработает.

---

## Интеграция со State Tree

### Вариант A: Один универсальный Task

Создать `FStateTreeTask_UseAbility`:
1. В `EnterState()`: вызвать `AbilityComponent->GetBestAbility()`, если есть → `TryActivateByIndex()`
2. В `Tick()`: проверять `AbilityComponent->IsAnyAbilityActive()`
3. Когда `IsAnyAbilityActive() == false` → вернуть `Succeeded`

### Вариант B: Через Blueprint Task

В State Tree Blueprint Task:
1. Получить `AbilityComponent` с актора
2. Вызвать `GetBestAbility()`
3. Если вернул валидный → `TryActivateByIndex()`
4. Ждать пока `IsAnyAbilityActive() == false`
5. Вернуть успех

### Вариант C: По конкретному классу

Для точного контроля — отдельный стейт на каждую способность:
```
State: Melee Attack
  Condition: Player дистанция < 200
  Task: AbilityComponent->TryActivateByClass(UFP_Ability_MeleeAttack)

State: Poison Spit
  Condition: Player дистанция > 500 И < 2000
  Task: AbilityComponent->TryActivateByClass(UFP_Ability_PoisonSpit)

State: Explode
  Condition: HP < 20%
  Task: AbilityComponent->TryActivateByClass(UFP_Ability_Explode)
```

### Рекомендация

Начни с **Варианта B** (Blueprint Task) — быстро, наглядно, можно протестировать. Когда система заработает — перенеси в C++ Task если нужна производительность.

---

## Пример потока данных

```
State Tree: "Игрок в радиусе 200 → атаковать"
    │
    ▼
AbilityComponent->GetBestAbility()
    │ возвращает UFP_Ability_MeleeAttack (Priority 5, CanActivate = true)
    │
    ▼
AbilityComponent->TryActivateByIndex(0)
    │
    ▼
UFP_Ability_MeleeAttack::CanActivate()
    │ Не активна? ✅  Не на кулдауне? ✅  Владелец жив? ✅
    │
    ▼
UFP_Ability_MeleeAttack::Activate()
    │ bIsActive = true
    │ AnimInstance->Montage_Play(ActivationMontage)
    │ UGameplayStatics::PlaySoundAtLocation(ActivationSound)
    │
    │ ... монтаж играет ...
    │
    │ AnimNotify "AttackHit" срабатывает:
    │   SphereTrace → нашёл игрока
    │   IFP_Damageable::Execute_ReceiveDamage(Player, 20.f)
    │
    │ ... монтаж закончился ...
    │
    ▼
UFP_Ability_MeleeAttack::OnCompleted()
    │ bIsActive = false
    │ StartCooldown(1.5 сек)
    │ OnAbilityFinished.Broadcast()
    │
    ▼
AbilityComponent → ActiveAbility = nullptr
State Tree Task → получил завершение → Succeeded → переход к следующему стейту
```

---

## Порядок реализации (пошагово)

### Этап 1: Фундамент
1. Создать `UFP_AbilityDataAsset` — все поля из таблицы выше
2. Создать `UFP_AbilityBase` — Initialize, CanActivate, Activate, Deactivate, OnCompleted, кулдаун, делегат
3. Создать `UFP_AbilityComponent` — InitializeAbilities, TryActivateByIndex, GetBestAbility, CancelActiveAbility

### Этап 2: Подключение
4. Добавить `TArray<UFP_AbilityDataAsset*> Abilities` в `UFP_ZombieConfig`
5. Добавить `UFP_AbilityComponent` в `AFP_BaseZombie` (конструктор + BeginPlay)
6. Раскомментировать строки в `FP_BaseZombie.h` которые уже подготовлены

### Этап 3: Первая способность
7. Создать `UFP_Ability_MeleeAttack` — наследник UFP_AbilityBase
8. Создать DataAsset `DA_Ability_MeleeAttack` в редакторе
9. Добавить в ZombieConfig → массив Abilities
10. **Протестировать** — вызвать TryActivateByIndex(0) напрямую из BeginPlay или по таймеру

### Этап 4: AI интеграция
11. Создать State Tree Task (Blueprint) который вызывает GetBestAbility → TryActivate
12. Добавить стейт в State Tree зомби
13. **Протестировать** — зомби подходит → атакует → кулдаун → снова атакует

### Этап 5: Остальные способности
14. `UFP_Ability_Explode` + DataAsset + тест
15. `UFP_Ability_PoisonSpit` + `AFP_PoisonProjectile` + DataAsset + тест
16. `UFP_Ability_Scream` + DataAsset + тест

### Этап 6: Полировка
17. Удалить старые захардкоженные поля атаки из `AFP_BaseZombie`
18. Настроить приоритеты способностей в DataAssets
19. Настроить условия в State Tree (расстояние, HP порог)
20. Добавить VFX/SFX через DataAsset

---

## Какие типы зомби получатся

| Тип | Abilities в ZombieConfig | Поведение |
|-----|--------------------------|-----------|
| Обычный зомби | `[MeleeAttack]` | Подходит → бьёт |
| Бегун | `[MeleeAttack, LeapAttack]` | Далеко → прыгает, близко → бьёт |
| Бомбер | `[MeleeAttack, Explode]` | Бьёт, при низком HP → взрыв |
| Плевун | `[PoisonSpit, MeleeAttack]` | Далеко → плюёт, близко → бьёт |
| Крикун | `[Scream, MeleeAttack]` | Периодически → крик, близко → бьёт |
| Элитный | `[PoisonSpit, Scream, MeleeAttack, Explode]` | Всё умеет, AI выбирает по ситуации |

Одна система, разные комбинации DataAssets — новый тип зомби создаётся **без единой строки кода**, только настройкой ассетов в редакторе.

