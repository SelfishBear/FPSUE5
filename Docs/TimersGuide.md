# Таймеры в Unreal Engine — Полный гайд

## Оглавление
- [Основы](#основы)
- [Запуск таймера](#запуск-таймера)
- [Остановка таймера](#остановка-таймера)
- [Проверка состояния](#проверка-состояния)
- [Получение информации](#получение-информации)
- [Паузы и возобновление](#паузы-и-возобновление)
- [Примеры использования](#примеры-использования)
- [Blueprint](#blueprint)
- [Шпаргалка](#шпаргалка)

---

## Основы

Таймеры управляются через `FTimerManager`, доступный из любого `AActor` или `UWorld`.

```cpp
// Получение TimerManager
GetWorldTimerManager()          // из AActor
GetWorld()->GetTimerManager()   // из UObject (если есть доступ к World)
```

Хэндл таймера — `FTimerHandle` — используется для управления таймером после запуска.

```cpp
// Объявление в .h файле
FTimerHandle MyTimerHandle;
```

---

## Запуск таймера

### SetTimer — вызов метода по имени

```cpp
// Вызвать MyFunction через 2 секунды, один раз
GetWorldTimerManager().SetTimer(
    MyTimerHandle,
    this,
    &AMyActor::MyFunction,
    2.0f,       // Rate — интервал в секундах
    false       // bLoop — зациклить?
);
```

### SetTimer — зацикленный

```cpp
// Вызывать MyFunction каждые 0.5 секунд бесконечно
GetWorldTimerManager().SetTimer(
    MyTimerHandle,
    this,
    &AMyActor::MyFunction,
    0.5f,
    true        // bLoop = true
);
```

### SetTimer — зацикленный с задержкой первого вызова

```cpp
// Каждые 1 секунду, но первый вызов через 3 секунды
GetWorldTimerManager().SetTimer(
    MyTimerHandle,
    this,
    &AMyActor::MyFunction,
    1.0f,       // Rate
    true,       // bLoop
    3.0f        // FirstDelay — задержка перед первым вызовом
);
```

### SetTimer — с лямбдой (FTimerDelegate)

```cpp
FTimerDelegate TimerDelegate;
TimerDelegate.BindLambda([this]()
{
    UE_LOG(LogTemp, Warning, TEXT("Timer fired!"));
    Health -= 10.f;
});

GetWorldTimerManager().SetTimer(MyTimerHandle, TimerDelegate, 1.0f, true);
```

### SetTimer — с лямбдой (короткая запись)

```cpp
GetWorldTimerManager().SetTimer(MyTimerHandle, FTimerDelegate::CreateLambda([this]()
{
    UE_LOG(LogTemp, Warning, TEXT("Boom!"));
}), 2.0f, false);
```

### SetTimer — с параметрами через делегат

```cpp
FTimerDelegate TimerDelegate;
TimerDelegate.BindUFunction(this, FName("MyFunctionWithParams"), 42, FString("Hello"));

GetWorldTimerManager().SetTimer(MyTimerHandle, TimerDelegate, 1.0f, false);
```

Функция должна быть `UFUNCTION`:

```cpp
UFUNCTION()
void MyFunctionWithParams(int32 Value, FString Text);
```

### SetTimerForNextTick — вызов на следующем тике

```cpp
// Вызвать функцию на следующем кадре
GetWorldTimerManager().SetTimerForNextTick(this, &AMyActor::MyFunction);

// С лямбдой
GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
{
    UE_LOG(LogTemp, Warning, TEXT("Next tick!"));
}));
```

---

## Остановка таймера

### ClearTimer — остановить и сбросить

```cpp
GetWorldTimerManager().ClearTimer(MyTimerHandle);
```

### ClearAllTimersForObject — остановить все таймеры объекта

```cpp
GetWorldTimerManager().ClearAllTimersForObject(this);
```

---

## Проверка состояния

### IsTimerActive — работает ли таймер

```cpp
bool bActive = GetWorldTimerManager().IsTimerActive(MyTimerHandle);
```

### IsTimerPaused — на паузе ли

```cpp
bool bPaused = GetWorldTimerManager().IsTimerPaused(MyTimerHandle);
```

### TimerExists — существует ли таймер (активный или на паузе)

```cpp
bool bExists = GetWorldTimerManager().TimerExists(MyTimerHandle);
```

---

## Получение информации

### GetTimerElapsed — сколько прошло с последнего срабатывания

```cpp
float Elapsed = GetWorldTimerManager().GetTimerElapsed(MyTimerHandle);
```

### GetTimerRemaining — сколько осталось до следующего срабатывания

```cpp
float Remaining = GetWorldTimerManager().GetTimerRemaining(MyTimerHandle);
```

### GetTimerRate — интервал таймера

```cpp
float Rate = GetWorldTimerManager().GetTimerRate(MyTimerHandle);
```

---

## Паузы и возобновление

### PauseTimer

```cpp
GetWorldTimerManager().PauseTimer(MyTimerHandle);
```

### UnPauseTimer

```cpp
GetWorldTimerManager().UnPauseTimer(MyTimerHandle);
```

---

## Примеры использования

### Перезарядка оружия

```cpp
// .h
FTimerHandle ReloadTimerHandle;

UPROPERTY(EditDefaultsOnly, Category = "Weapon")
float ReloadTime = 2.0f;

void StartReload();
void FinishReload();

// .cpp
void AWeapon::StartReload()
{
    bIsReloading = true;
    GetWorldTimerManager().SetTimer(
        ReloadTimerHandle,
        this,
        &AWeapon::FinishReload,
        ReloadTime,
        false
    );
}

void AWeapon::FinishReload()
{
    bIsReloading = false;
    CurrentAmmo = MaxAmmo;
}
```

### Регенерация здоровья

```cpp
// .h
FTimerHandle RegenTimerHandle;

void StartHealthRegen();
void RegenTick();

// .cpp
void AMyCharacter::StartHealthRegen()
{
    // Каждую 1 секунду восстанавливать HP, первый тик через 3 секунды
    GetWorldTimerManager().SetTimer(
        RegenTimerHandle,
        this,
        &AMyCharacter::RegenTick,
        1.0f,
        true,
        3.0f
    );
}

void AMyCharacter::RegenTick()
{
    Health = FMath::Min(Health + 5.f, MaxHealth);

    if (Health >= MaxHealth)
    {
        GetWorldTimerManager().ClearTimer(RegenTimerHandle);
    }
}
```

### Отложенное действие (бомба)

```cpp
void ABomb::Activate()
{
    GetWorldTimerManager().SetTimer(
        ExplosionTimerHandle,
        this,
        &ABomb::Explode,
        FuseTime,
        false
    );
}

void ABomb::Explode()
{
    // VFX, звук, урон
    UGameplayStatics::ApplyRadialDamage(
        this, Damage, GetActorLocation(),
        ExplosionRadius, DamageType,
        TArray<AActor*>(), this
    );
    Destroy();
}
```

### Автоматическая стрельба (зацикленный таймер)

```cpp
void AWeapon::StartFiring()
{
    // Сразу стрелять + повторять каждые FireRate секунд
    GetWorldTimerManager().SetTimer(
        FireTimerHandle,
        this,
        &AWeapon::Fire,
        FireRate,
        true,
        0.0f  // без задержки первого выстрела
    );
}

void AWeapon::StopFiring()
{
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
}
```

### Таймер из UObject (не Actor)

```cpp
// UObject не имеет GetWorldTimerManager(), нужен доступ к World
void UMySubsystem::StartTimer()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            MyTimerHandle,
            this,
            &UMySubsystem::OnTimerFired,
            1.0f,
            false
        );
    }
}
```

### Отмена таймера при уничтожении

```cpp
void AMyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    GetWorldTimerManager().ClearAllTimersForObject(this);
}
```

---

## Blueprint

### Ноды

| Нода | Описание |
|---|---|
| `Set Timer by Function Name` | Запуск таймера по имени функции |
| `Set Timer by Event` | Запуск таймера с Custom Event (удобнее) |
| `Clear and Invalidate Timer by Handle` | Остановить таймер |
| `Pause Timer by Handle` | Пауза |
| `Unpause Timer by Handle` | Снять паузу |
| `Is Timer Active by Handle` | Проверка активности |
| `Get Timer Elapsed Time by Handle` | Прошедшее время |
| `Get Timer Remaining Time by Handle` | Оставшееся время |
| `Set Timer for Next Tick by Event` | Вызов на следующем кадре |

### Set Timer by Event (рекомендуется)

```
Set Timer by Event
├── Time: 2.0
├── Looping: false
├── Event → привязанный Custom Event
└── Return Value → Timer Handle (сохрани в переменную!)
```

### Set Timer by Function Name

```
Set Timer by Function Name
├── Object: Self
├── Function Name: "MyFunction"
├── Time: 1.0
├── Looping: true
└── Return Value → Timer Handle
```

---

## Шпаргалка

### Все методы FTimerManager

| Метод | Описание |
|---|---|
| `SetTimer(Handle, Object, &Func, Rate, bLoop)` | Запуск таймера |
| `SetTimer(Handle, Object, &Func, Rate, bLoop, FirstDelay)` | С задержкой первого вызова |
| `SetTimer(Handle, Delegate, Rate, bLoop)` | С делегатом/лямбдой |
| `SetTimerForNextTick(Object, &Func)` | Вызов на следующем тике |
| `SetTimerForNextTick(Delegate)` | С делегатом на следующем тике |
| `ClearTimer(Handle)` | Остановить таймер |
| `ClearAllTimersForObject(Object)` | Остановить все таймеры объекта |
| `PauseTimer(Handle)` | Поставить на паузу |
| `UnPauseTimer(Handle)` | Снять с паузы |
| `IsTimerActive(Handle)` | Активен? |
| `IsTimerPaused(Handle)` | На паузе? |
| `TimerExists(Handle)` | Существует? |
| `GetTimerElapsed(Handle)` | Прошло времени |
| `GetTimerRemaining(Handle)` | Осталось времени |
| `GetTimerRate(Handle)` | Интервал таймера |

### Параметры SetTimer

| Параметр | Тип | Описание |
|---|---|---|
| `InOutHandle` | `FTimerHandle&` | Хэндл для управления |
| `InObj` | `UObject*` | Объект-владелец |
| `InTimerMethod` | `void (T::*)()` | Указатель на метод |
| `InRate` | `float` | Интервал в секундах |
| `InbLoop` | `bool` | Зацикленный? |
| `InFirstDelay` | `float` | Задержка первого вызова (-1 = равен Rate) |

