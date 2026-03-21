# Архитектура системы оружия

## Философия
- **UObject** — чистая логика (урон, скорострельность, разброс, патроны, стейт-машина стрельбы)
- **AActor** — чистый визуал (меш, анимации, эффекты, звуки)
- Логика **не знает** о визуале. Визуал **читает** данные из логики и реагирует на делегаты.

---

## Диаграмма классов

```
UObject
 └── UWeaponBase                    ← базовая логика оружия
      ├── UWeaponRanged             ← стрельба (hitscan / projectile)
      └── UWeaponMelee              ← ближний бой (trace / overlap)

AActor
 └── AWeaponVisualBase              ← базовый визуал оружия
      ├── AWeaponVisualRanged       ← визуал огнестрела (вспышки, гильзы)
      └── AWeaponVisualMelee        ← визуал мили (trail-эффект, swing)

UDataAsset
 └── UWeaponDataAsset               ← конфигурация: статы + ссылка на класс визуала
```

---

## 1. UWeaponBase (UObject)

### Ответственность
Хранит рантайм-состояние оружия и выполняет всю игровую логику.

### Ключевые данные
| Поле | Тип | Описание |
|------|-----|----------|
| `OwnerCharacter` | `TWeakObjectPtr<AFP_BaseCharacter>` | Владелец оружия (слабая ссылка) |
| `WeaponData` | `UWeaponDataAsset*` | Ссылка на конфигурацию (DataAsset) |
| `CurrentAmmo` | `int32` | Текущие патроны в магазине |
| `ReserveAmmo` | `int32` | Запас патронов |
| `bCanFire` | `bool` | Флаг готовности к стрельбе |
| `WeaponState` | `EWeaponState` | Текущее состояние (Idle, Firing, Reloading, Equipping) |

### Ключевые методы
```
StartFire()          — начать стрельбу (авто / одиночная / очередь)
StopFire()           — прекратить стрельбу
Reload()             — начать перезарядку
Equip()              — экипировать (запускает таймер equip-time)
Unequip()            — убрать оружие
CanFire() → bool     — проверка: есть патроны, не перезаряжается, cooldown прошёл
ConsumeAmmo()        — списать патрон
PerformFire()        — внутренний метод: line trace / spawn projectile
```

### Делегаты (для визуала)
```
OnFired              — выстрел произведён
OnReloadStarted      — начата перезарядка
OnReloadFinished     — перезарядка завершена
OnAmmoChanged        — изменилось кол-во патронов
OnEquipStarted       — начато экипирование
OnEquipFinished      — экипирование завершено
OnWeaponStateChanged — изменился стейт
```

### Важный нюанс
`UObject` не тикает сам по себе. Варианты:
- Владелец (персонаж) вызывает `WeaponBase->Tick(DeltaTime)` из своего `Tick`
- Использовать `FTimerManager` для таймеров (перезарядка, cooldown, equip)
- **Рекомендация**: `FTimerHandle` для всего — никакого ручного тика не нужно

---

## 2. AWeaponVisualBase (AActor)

### Ответственность
Только отображение: меш, звуки, VFX, анимации от первого лица.

### Ключевые компоненты
| Компонент | Тип | Описание |
|-----------|-----|----------|
| `WeaponMesh` | `USkeletalMeshComponent` | Меш оружия |
| `MuzzlePoint` | `USceneComponent` | Точка дула (для VFX/спавна) |

### Ключевые методы
```
Initialize(UWeaponBase* Logic)   — привязаться к логике, подписаться на делегаты
PlayFireEffects()                — вспышка, звук выстрела, анимация
PlayReloadAnimation()            — анимация перезарядки
PlayEquipAnimation()             — анимация доставания
```

### Подписка на делегаты
При `Initialize` визуал подписывается на делегаты логики:
- `OnFired` → `PlayFireEffects()`
- `OnReloadStarted` → `PlayReloadAnimation()`
- `OnEquipStarted` → `PlayEquipAnimation()`

### Кто владеет визуалом?
Визуальный актор **Attach** к мешу персонажа (`MeshOffsetRoot` или сокет на руках).
Спавнится и уничтожается при смене оружия.

---

## 3. UWeaponDataAsset (UDataAsset)

### Ответственность
Хранит **статическую конфигурацию** оружия. Редактируется в редакторе.

### Данные
| Поле | Тип | Описание |
|------|-----|----------|
| `WeaponName` | `FText` | Название |
| `Damage` | `float` | Урон за выстрел |
| `FireRate` | `float` | Скорострельность (выстрелов/сек) |
| `MaxAmmo` | `int32` | Макс. патронов в магазине |
| `MaxReserveAmmo` | `int32` | Макс. запас |
| `ReloadTime` | `float` | Время перезарядки |
| `EquipTime` | `float` | Время экипирования |
| `SpreadAngle` | `float` | Базовый разброс |
| `FireMode` | `EFireMode` | Auto / Semi / Burst |
| `bIsHitscan` | `bool` | Hitscan или Projectile |
| `ProjectileClass` | `TSubclassOf<AActor>` | Класс снаряда (если projectile) |
| `WeaponVisualClass` | `TSubclassOf<AWeaponVisualBase>` | Какой визуал спавнить |
| `WeaponLogicClass` | `TSubclassOf<UWeaponBase>` | Какой класс логики создавать |

---

## 4. Enum'ы

```cpp
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
    Idle,
    Firing,
    Reloading,
    Equipping,
    Unequipping
};

UENUM(BlueprintType)
enum class EFireMode : uint8
{
    Semi,       // одиночная
    Auto,       // автоматическая
    Burst       // очередь (2-3 выстрела)
};
```

---

## 5. Inventory / Weapon Manager

Нужен компонент на персонаже, который управляет слотами оружия.

### UWeaponManagerComponent (UActorComponent)

| Поле | Тип | Описание |
|------|-----|----------|
| `WeaponSlots` | `TArray<UWeaponBase*>` | Массив оружия (логика) |
| `CurrentWeaponIndex` | `int32` | Текущий слот |
| `CurrentVisual` | `AWeaponVisualBase*` | Текущий заспавненный визуал |

### Методы
```
AddWeapon(UWeaponDataAsset*)     — создать UWeaponBase, добавить в слот
RemoveWeapon(int32 Index)        — удалить из слота
SwitchWeapon(int32 Index)        — сменить оружие (Unequip → Equip)
GetCurrentWeapon() → UWeaponBase*
```

### Логика смены оружия
1. `CurrentWeapon->Unequip()` → ждём `OnUnequipFinished`
2. Уничтожить текущий `AWeaponVisualBase`
3. Сменить `CurrentWeaponIndex`
4. Спавнить новый `AWeaponVisualBase` из `WeaponData->WeaponVisualClass`
5. `Visual->Initialize(NewWeapon)`
6. `NewWeapon->Equip()` → ждём `OnEquipFinished`

---

## 6. Связь с персонажем

```
AFP_PlayerCharacter
 ├── UWeaponManagerComponent        ← управление слотами
 │    ├── UWeaponBase* [0]          ← логика пистолета (UObject)
 │    ├── UWeaponBase* [1]          ← логика автомата (UObject)
 │    └── AWeaponVisualBase*        ← текущий визуал (Actor, attached к мешу)
 │
 ├── Input: Fire → WeaponManager → CurrentWeapon->StartFire()
 ├── Input: Reload → WeaponManager → CurrentWeapon->Reload()
 └── Input: SwitchWeapon → WeaponManager->SwitchWeapon(Index)
```

---

## 7. Порядок реализации

| Шаг | Что делать |
|-----|-----------|
| 1 | Создать `EWeaponState`, `EFireMode` |
| 2 | Создать `UWeaponDataAsset` со всеми полями |
| 3 | Создать `UWeaponBase` — базовая логика с делегатами |
| 4 | Создать `AWeaponVisualBase` — базовый визуал с подпиской на делегаты |
| 5 | Создать `UWeaponManagerComponent` — управление слотами |
| 6 | Интегрировать `UWeaponManagerComponent` в `AFP_PlayerCharacter` |
| 7 | Создать `UWeaponRanged` — hitscan стрельба |
| 8 | Создать `AWeaponVisualRanged` — VFX стрельбы |
| 9 | Создать `UWeaponDataAsset` ассет для тестового оружия в редакторе |
| 10 | Тестирование: стрельба, перезарядка, смена оружия |
| 11 | Создать `UWeaponMelee` + `AWeaponVisualMelee` (по необходимости) |

---

## 8. Плюсы архитектуры

- **Логика не зависит от визуала** — можно тестировать без меша, можно менять визуал на лету
- **DataAsset** — удобная конфигурация из редактора, можно создать 50 пушек без кода
- **Делегаты** — визуал реактивно реагирует на события логики, loose coupling
- **UObject для логики** — лёгкий, не нагружает сцену акторами, легко сериализуется
- **WeaponManager** — централизованное управление, легко расширять (колесо оружия, дроп, пикап)

## 9. Потенциальные подводные камни

| Проблема | Решение |
|----------|---------|
| UObject не имеет `GetWorld()` | Передать World через `OwnerCharacter->GetWorld()` или переопределить `GetWorld()` |
| UObject не тикает | Использовать `FTimerManager` от World |
| GC может удалить UObject | Хранить в `UPROPERTY()` массиве в компоненте |
| Мультиплеер: UObject не реплицируется по умолчанию | Реплицировать через владельца или использовать `ReplicatedSubobject` |

