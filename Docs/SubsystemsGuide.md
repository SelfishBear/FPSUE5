﻿# Подсистемы (Subsystems) в Unreal Engine — Полный гайд

## Оглавление
- [Что такое подсистема](#что-такое-подсистема)
- [Зачем нужны подсистемы](#зачем-нужны-подсистемы)
- [Типы подсистем](#типы-подсистем)
- [UEngineSubsystem](#uenginesubsystem)
- [UEditorSubsystem](#ueditorsubsystem)
- [UGameInstanceSubsystem](#ugameinstancesubsystem)
- [UWorldSubsystem](#uworldsubsystem)
- [ULocalPlayerSubsystem](#ulocalplayersubsystem)
- [Жизненный цикл подсистемы](#жизненный-цикл-подсистемы)
- [Создание своей подсистемы — пошагово](#создание-своей-подсистемы--пошагово)
- [Получение ссылки на подсистему](#получение-ссылки-на-подсистему)
- [Tick в подсистемах](#tick-в-подсистемах)
- [Подсистемы и Blueprint](#подсистемы-и-blueprint)
- [Практические примеры](#практические-примеры)
  - [Система сохранений (GameInstanceSubsystem)](#система-сохранений-gameinstancesubsystem)
  - [Система объективов/квестов (WorldSubsystem)](#система-объективовквестов-worldsubsystem)
  - [Система настроек игрока (LocalPlayerSubsystem)](#система-настроек-игрока-localplayersubsystem)
  - [Пул объектов (WorldSubsystem)](#пул-объектов-worldsubsystem)
  - [Система аудио-окружения (WorldSubsystem)](#система-аудио-окружения-worldsubsystem)
- [Подсистемы vs Singleton vs GameMode vs Manager](#подсистемы-vs-singleton-vs-gamemode-vs-manager)
- [Взаимодействие подсистем между собой](#взаимодействие-подсистем-между-собой)
- [Подсистемы и сеть (Multiplayer)](#подсистемы-и-сеть-multiplayer)
- [Антипаттерны и ошибки](#антипаттерны-и-ошибки)
- [Шпаргалка — когда какой тип использовать](#шпаргалка--когда-какой-тип-использовать)
- [Шпаргалка — полный API](#шпаргалка--полный-api)

---

## Что такое подсистема

Подсистема — это автоматически создаваемый и управляемый движком синглтон-объект, привязанный к определённому «владельцу» (Engine, GameInstance, World, LocalPlayer). Движок сам создаёт, инициализирует и уничтожает подсистему — тебе не нужно писать `NewObject`, `Spawn`, искать место для инициализации.

```
┌──────────────────────────────────────────────────────┐
│  Владелец создаётся (World, GameInstance и т.д.)     │
│         ↓                                            │
│  Движок автоматически создаёт все подсистемы         │
│  зарегистрированные для этого типа владельца         │
│         ↓                                            │
│  Initialize() вызывается на каждой подсистеме        │
│         ↓                                            │
│  Подсистемы живут и работают                         │
│         ↓                                            │
│  Владелец уничтожается                               │
│         ↓                                            │
│  Deinitialize() вызывается → подсистемы удаляются    │
└──────────────────────────────────────────────────────┘
```

---

## Зачем нужны подсистемы

| Проблема | Решение через подсистему |
|---|---|
| Нужен глобальный менеджер, но не хочется писать синглтон | Подсистема — готовый управляемый синглтон |
| Менеджер должен жить пока есть мир (уровень) | `UWorldSubsystem` |
| Менеджер должен жить между уровнями | `UGameInstanceSubsystem` |
| Данные привязаны к конкретному игроку (split-screen) | `ULocalPlayerSubsystem` |
| Нужна глобальная система на весь движок | `UEngineSubsystem` |
| Надоело делать `FindActor`, `GetActorOfClass` для менеджеров | Подсистема доступна из любого места через 1 вызов |
| Надоело управлять порядком инициализации руками | Движок делает всё сам |

---

## Типы подсистем

```
                    USubsystem
                        │
           ┌────────────┴────────────┐
           │                         │
   UDynamicSubsystem          (базовые типы)
           │
           ├── UEngineSubsystem          → Владелец: GEngine (UEngine)
           ├── UEditorSubsystem          → Владелец: GEditor (UEditorEngine)
           ├── UGameInstanceSubsystem    → Владелец: UGameInstance
           ├── UWorldSubsystem           → Владелец: UWorld
           └── ULocalPlayerSubsystem     → Владелец: ULocalPlayer
```

### Сравнительная таблица

| Тип | Владелец | Время жизни | Кол-во экземпляров | Типичное применение |
|---|---|---|---|---|
| `UEngineSubsystem` | `UEngine` | Весь процесс | 1 | Аналитика, логирование |
| `UEditorSubsystem` | `UEditorEngine` | Пока открыт редактор | 1 | Инструменты редактора |
| `UGameInstanceSubsystem` | `UGameInstance` | Между уровнями | 1 | Сохранения, настройки, достижения |
| `UWorldSubsystem` | `UWorld` | Пока загружен мир | 1 на мир | Спавн-менеджер, пул, квесты |
| `ULocalPlayerSubsystem` | `ULocalPlayer` | Пока существует игрок | 1 на игрока | Настройки UI, управление камерой |

---

## UEngineSubsystem

Живёт всё время работы движка. Создаётся при старте `GEngine`, уничтожается при закрытии.

```cpp
// MyAnalyticsSubsystem.h
#pragma once
#include "Subsystems/EngineSubsystem.h"
#include "MyAnalyticsSubsystem.generated.h"

UCLASS()
class FPSGAME_API UMyAnalyticsSubsystem : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void TrackEvent(const FString& EventName);

private:
    TArray<FString> EventLog;
};
```

```cpp
// MyAnalyticsSubsystem.cpp
#include "MyAnalyticsSubsystem.h"

void UMyAnalyticsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("Analytics subsystem initialized"));
}

void UMyAnalyticsSubsystem::Deinitialize()
{
    // Отправить накопленные события
    UE_LOG(LogTemp, Log, TEXT("Analytics subsystem deinitialized. Events: %d"), EventLog.Num());
    Super::Deinitialize();
}

void UMyAnalyticsSubsystem::TrackEvent(const FString& EventName)
{
    EventLog.Add(EventName);
}
```

**Получение:**
```cpp
UMyAnalyticsSubsystem* Analytics = GEngine->GetEngineSubsystem<UMyAnalyticsSubsystem>();
Analytics->TrackEvent(TEXT("PlayerDied"));
```

---

## UEditorSubsystem

Живёт только в редакторе. Идеально для кастомных инструментов.

```cpp
#pragma once
#include "Subsystems/EditorSubsystem.h"
#include "MyEditorToolSubsystem.generated.h"

UCLASS()
class FPSGAMEEDITOR_API UMyEditorToolSubsystem : public UEditorSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Editor Tools")
    void OrganizeActorsByType();
};
```

**Получение:**
```cpp
#include "Editor.h"
UMyEditorToolSubsystem* Tool = GEditor->GetEditorSubsystem<UMyEditorToolSubsystem>();
```

> ⚠️ `UEditorSubsystem` доступен только в Editor-модулях. Не используй в Runtime-коде!

---

## UGameInstanceSubsystem

Живёт пока жив `UGameInstance` — **переживает смену уровней**. Идеален для данных, которые должны сохраняться между картами.

```cpp
// FP_SaveSubsystem.h
#pragma once
#include "Subsystems/GameInstanceSubsystem.h"
#include "FP_SaveSubsystem.generated.h"

UCLASS()
class FPSGAME_API UFP_SaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Опционально: контроль создания
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveGame(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "Save")
    void LoadGame(const FString& SlotName);

    UFUNCTION(BlueprintPure, Category = "Save")
    int32 GetTotalPlayTime() const { return TotalPlayTimeSeconds; }

private:
    int32 TotalPlayTimeSeconds = 0;
};
```

```cpp
// FP_SaveSubsystem.cpp
#include "FP_SaveSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UFP_SaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("Save subsystem initialized"));
}

void UFP_SaveSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("Save subsystem deinitialized"));
    Super::Deinitialize();
}

void UFP_SaveSubsystem::SaveGame(const FString& SlotName)
{
    // Логика сохранения
    UE_LOG(LogTemp, Log, TEXT("Game saved to slot: %s"), *SlotName);
}

void UFP_SaveSubsystem::LoadGame(const FString& SlotName)
{
    // Логика загрузки
    UE_LOG(LogTemp, Log, TEXT("Game loaded from slot: %s"), *SlotName);
}
```

**Получение:**
```cpp
// Из любого AActor
UGameInstance* GI = GetGameInstance();
UFP_SaveSubsystem* SaveSys = GI->GetSubsystem<UFP_SaveSubsystem>();

// Или короче
UFP_SaveSubsystem* SaveSys = GetGameInstance()->GetSubsystem<UFP_SaveSubsystem>();
```

---

## UWorldSubsystem

Живёт пока загружен мир (уровень). **Уничтожается при смене уровня** и создаётся заново на новом.

```cpp
// FP_WaveSubsystem.h
#pragma once
#include "Subsystems/WorldSubsystem.h"
#include "FP_WaveSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveChanged, int32, NewWaveNumber);

UCLASS()
class FPSGAME_API UFP_WaveSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Контроль: не создавать в превью-мирах редактора
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    UFUNCTION(BlueprintCallable, Category = "Waves")
    void StartNextWave();

    UFUNCTION(BlueprintPure, Category = "Waves")
    int32 GetCurrentWave() const { return CurrentWave; }

    UPROPERTY(BlueprintAssignable, Category = "Waves")
    FOnWaveChanged OnWaveChanged;

private:
    int32 CurrentWave = 0;
};
```

```cpp
// FP_WaveSubsystem.cpp
#include "FP_WaveSubsystem.h"

void UFP_WaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    CurrentWave = 0;
    UE_LOG(LogTemp, Log, TEXT("Wave subsystem initialized"));
}

void UFP_WaveSubsystem::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("Wave subsystem deinitialized at wave %d"), CurrentWave);
    Super::Deinitialize();
}

bool UFP_WaveSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    // Не создавать в превью-мирах редактора
    if (const UWorld* World = Cast<UWorld>(Outer))
    {
        return World->IsGameWorld();
    }
    return false;
}

void UFP_WaveSubsystem::StartNextWave()
{
    CurrentWave++;
    OnWaveChanged.Broadcast(CurrentWave);
    UE_LOG(LogTemp, Log, TEXT("Wave %d started"), CurrentWave);
}
```

**Получение:**
```cpp
// Из AActor
UFP_WaveSubsystem* WaveSys = GetWorld()->GetSubsystem<UFP_WaveSubsystem>();

// Из UObject (если есть доступ к World)
UWorld* World = GEngine->GetWorldFromContextObject(SomeObject, EGetWorldErrorMode::LogAndReturnNull);
UFP_WaveSubsystem* WaveSys = World->GetSubsystem<UFP_WaveSubsystem>();
```

---

## ULocalPlayerSubsystem

Один экземпляр на каждого `ULocalPlayer`. В обычной игре — один. В split-screen — по одному на игрока.

```cpp
// FP_PlayerSettingsSubsystem.h
#pragma once
#include "Subsystems/LocalPlayerSubsystem.h"
#include "FP_PlayerSettingsSubsystem.generated.h"

UCLASS()
class FPSGAME_API UFP_PlayerSettingsSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetMouseSensitivity(float NewSensitivity);

    UFUNCTION(BlueprintPure, Category = "Settings")
    float GetMouseSensitivity() const { return MouseSensitivity; }

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void ToggleHUD(bool bShow);

private:
    float MouseSensitivity = 1.0f;
    bool bHUDVisible = true;
};
```

**Получение:**
```cpp
// Из PlayerController
APlayerController* PC = GetWorld()->GetFirstPlayerController();
UFP_PlayerSettingsSubsystem* Settings = ULocalPlayer::GetSubsystem<UFP_PlayerSettingsSubsystem>(PC->GetLocalPlayer());

// Из ULocalPlayer напрямую
ULocalPlayer* LP = PC->GetLocalPlayer();
UFP_PlayerSettingsSubsystem* Settings = LP->GetSubsystem<UFP_PlayerSettingsSubsystem>();
```

---

## Жизненный цикл подсистемы

```
┌─────────────────────────────────────────────────────────────────┐
│ 1. Владелец создаётся (UWorld, UGameInstance, и т.д.)           │
│                                                                 │
│ 2. Движок перебирает все UCLASS наследники нужного типа         │
│    подсистемы через рефлексию                                   │
│                                                                 │
│ 3. Для каждого класса вызывается ShouldCreateSubsystem()        │
│    ├── return true  → подсистема будет создана                  │
│    └── return false → подсистема пропускается                   │
│                                                                 │
│ 4. NewObject<>() — движок создаёт экземпляр                    │
│                                                                 │
│ 5. Initialize(FSubsystemCollectionBase& Collection) вызывается  │
│    • Collection позволяет объявить зависимости                  │
│    • Подписка на делегаты, загрузка данных                      │
│                                                                 │
│ 6. Подсистема живёт и работает                                  │
│    • Доступна через GetSubsystem<T>()                           │
│    • Может тикать (если реализован Tick)                        │
│                                                                 │
│ 7. Владелец уничтожается                                        │
│                                                                 │
│ 8. Deinitialize() вызывается                                    │
│    • Отписка от делегатов, очистка ресурсов                     │
│                                                                 │
│ 9. Подсистема уничтожается GC                                   │
└─────────────────────────────────────────────────────────────────┘
```

### ShouldCreateSubsystem

Позволяет условно отключить подсистему:

```cpp
virtual bool ShouldCreateSubsystem(UObject* Outer) const override
{
    // Outer — это владелец (UWorld, UGameInstance и т.д.)

    // Пример: не создавать на dedicated-сервере
    if (const UWorld* World = Cast<UWorld>(Outer))
    {
        return !World->IsNetMode(NM_DedicatedServer);
    }
    return true;
}
```

### Initialize — зависимости между подсистемами

```cpp
void UFP_WaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Гарантирует, что UFP_SpawnSubsystem будет инициализирован ДО нас
    Collection.InitializeDependency<UFP_SpawnSubsystem>();

    // Теперь безопасно получить ссылку
    UFP_SpawnSubsystem* SpawnSys = GetWorld()->GetSubsystem<UFP_SpawnSubsystem>();
}
```

> ⚠️ Циклические зависимости приведут к крашу! A → B → A — запрещено.

---

## Создание своей подсистемы — пошагово

### Шаг 1: Создай .h файл

```cpp
// FP_ObjectPoolSubsystem.h
#pragma once
#include "Subsystems/WorldSubsystem.h"
#include "FP_ObjectPoolSubsystem.generated.h"

UCLASS()
class FPSGAME_API UFP_ObjectPoolSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    UFUNCTION(BlueprintCallable, Category = "Pool")
    AActor* GetPooledActor(TSubclassOf<AActor> ActorClass);

    UFUNCTION(BlueprintCallable, Category = "Pool")
    void ReturnToPool(AActor* Actor);

private:
    TMap<UClass*, TArray<AActor*>> Pool;
};
```

### Шаг 2: Создай .cpp файл

```cpp
// FP_ObjectPoolSubsystem.cpp
#include "FP_ObjectPoolSubsystem.h"

void UFP_ObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("Object Pool initialized"));
}

void UFP_ObjectPoolSubsystem::Deinitialize()
{
    Pool.Empty();
    Super::Deinitialize();
}

bool UFP_ObjectPoolSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (const UWorld* World = Cast<UWorld>(Outer))
    {
        return World->IsGameWorld();
    }
    return false;
}

AActor* UFP_ObjectPoolSubsystem::GetPooledActor(TSubclassOf<AActor> ActorClass)
{
    if (!ActorClass) return nullptr;

    TArray<AActor*>& ClassPool = Pool.FindOrAdd(ActorClass);

    // Ищем неактивного актора в пуле
    for (AActor* Actor : ClassPool)
    {
        if (IsValid(Actor) && Actor->IsHidden())
        {
            Actor->SetActorHiddenInGame(false);
            Actor->SetActorEnableCollision(true);
            Actor->SetActorTickEnabled(true);
            return Actor;
        }
    }

    // Нет свободных — спавним нового
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* NewActor = GetWorld()->SpawnActor<AActor>(ActorClass, FTransform::Identity, SpawnParams);
    if (NewActor)
    {
        ClassPool.Add(NewActor);
    }
    return NewActor;
}

void UFP_ObjectPoolSubsystem::ReturnToPool(AActor* Actor)
{
    if (!IsValid(Actor)) return;

    Actor->SetActorHiddenInGame(true);
    Actor->SetActorEnableCollision(false);
    Actor->SetActorTickEnabled(false);
}
```

### Шаг 3: Готово. Используй!

```cpp
// В любом Actor или компоненте
void AMyWeapon::SpawnProjectile()
{
    UFP_ObjectPoolSubsystem* Pool = GetWorld()->GetSubsystem<UFP_ObjectPoolSubsystem>();
    if (!Pool) return;

    AActor* Projectile = Pool->GetPooledActor(ProjectileClass);
    if (Projectile)
    {
        Projectile->SetActorLocationAndRotation(MuzzleLocation, MuzzleRotation);
    }
}
```

Никакого `NewObject`, `SpawnActor` менеджера, `FindActorOfClass` — подсистема просто **есть**.

---

## Получение ссылки на подсистему

### Таблица способов доступа

| Тип подсистемы | Из AActor | Из UObject | Из Blueprint |
|---|---|---|---|
| `UEngineSubsystem` | `GEngine->GetEngineSubsystem<T>()` | `GEngine->GetEngineSubsystem<T>()` | ✅ (если BlueprintCallable) |
| `UGameInstanceSubsystem` | `GetGameInstance()->GetSubsystem<T>()` | Через `UGameplayStatics::GetGameInstance()` | ✅ |
| `UWorldSubsystem` | `GetWorld()->GetSubsystem<T>()` | Нужен доступ к `UWorld*` | ✅ |
| `ULocalPlayerSubsystem` | Через `ULocalPlayer*` | Через `ULocalPlayer*` | ✅ |

### Подробные примеры

```cpp
// ===== UEngineSubsystem =====
// Из любого места (глобальный)
UMyEngineSubsystem* Sys = GEngine->GetEngineSubsystem<UMyEngineSubsystem>();


// ===== UGameInstanceSubsystem =====
// Из AActor
UMyGameSubsystem* Sys = GetGameInstance()->GetSubsystem<UMyGameSubsystem>();

// Из UObject (через World)
UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject);
UMyGameSubsystem* Sys = GI->GetSubsystem<UMyGameSubsystem>();

// Из UObject (через World контекст)
UMyGameSubsystem* Sys = UGameInstance::GetSubsystem<UMyGameSubsystem>(GetWorld()->GetGameInstance());


// ===== UWorldSubsystem =====
// Из AActor
UMyWorldSubsystem* Sys = GetWorld()->GetSubsystem<UMyWorldSubsystem>();

// Из GameMode
UMyWorldSubsystem* Sys = GetWorld()->GetSubsystem<UMyWorldSubsystem>();

// Из UObject, у которого есть Outer с миром
UWorld* World = GetOuter()->GetWorld(); // или GEngine->GetWorldFromContextObject(...)
UMyWorldSubsystem* Sys = World->GetSubsystem<UMyWorldSubsystem>();


// ===== ULocalPlayerSubsystem =====
// Из APlayerController
UMyPlayerSubsystem* Sys = GetLocalPlayer()->GetSubsystem<UMyPlayerSubsystem>();

// Статический метод
APlayerController* PC = GetWorld()->GetFirstPlayerController();
UMyPlayerSubsystem* Sys = ULocalPlayer::GetSubsystem<UMyPlayerSubsystem>(PC->GetLocalPlayer());
```

---

## Tick в подсистемах

По умолчанию подсистемы **не тикают**. Чтобы добавить Tick, реализуй интерфейс `FTickableGameObject`:

```cpp
// FP_AIDirectorSubsystem.h
#pragma once
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "FP_AIDirectorSubsystem.generated.h"

UCLASS()
class FPSGAME_API UFP_AIDirectorSubsystem : public UWorldSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    // Обязательно: без этого будет тикать даже в редакторе
    virtual bool IsTickable() const override { return !IsTemplate(); }

    // Опционально: тикать даже на паузе?
    virtual bool IsTickableWhenPaused() const override { return false; }

    // Опционально: тикать в редакторе?
    virtual bool IsTickableInEditor() const override { return false; }

    // Опционально: в каком мире тикать
    virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

private:
    float DifficultyMultiplier = 1.0f;
    float TimeSinceLastAdjustment = 0.0f;
};
```

```cpp
// FP_AIDirectorSubsystem.cpp
#include "FP_AIDirectorSubsystem.h"

void UFP_AIDirectorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    DifficultyMultiplier = 1.0f;
}

void UFP_AIDirectorSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

void UFP_AIDirectorSubsystem::Tick(float DeltaTime)
{
    TimeSinceLastAdjustment += DeltaTime;

    // Каждые 10 секунд корректируем сложность
    if (TimeSinceLastAdjustment >= 10.0f)
    {
        TimeSinceLastAdjustment = 0.0f;
        // Логика адаптивной сложности...
        UE_LOG(LogTemp, Verbose, TEXT("AI Director tick: difficulty = %.2f"), DifficultyMultiplier);
    }
}

TStatId UFP_AIDirectorSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UFP_AIDirectorSubsystem, STATGROUP_Tickables);
}
```

### Альтернатива: таймер вместо Tick

Если не нужен покадровый тик, используй таймер — это легче:

```cpp
void UFP_WaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Проверяем каждые 5 секунд
    GetWorld()->GetTimerManager().SetTimer(
        CheckTimerHandle, this, &UFP_WaveSubsystem::CheckWaveState,
        5.0f, true);
}

void UFP_WaveSubsystem::Deinitialize()
{
    GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
    Super::Deinitialize();
}
```

---

## Подсистемы и Blueprint

Чтобы подсистема была доступна из Blueprint, пометь функции:

```cpp
UCLASS(BlueprintType)
class FPSGAME_API UFP_ScoreSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Вызываемая из BP
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Points);

    // Читаемая из BP
    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetTotalScore() const { return TotalScore; }

    // Событие для BP
    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnScoreChanged OnScoreChanged;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score", meta = (AllowPrivateAccess = "true"))
    int32 TotalScore = 0;
};
```

### Доступ из Blueprint

В Blueprint Graph:
1. Перетащи узел **Get Game Instance** (или **Get World**)
2. Протяни от него и найди **Get Subsystem** → выбери свой тип
3. От результата вызывай свои `BlueprintCallable` функции

```
[Get Game Instance] → [Get Subsystem (UFP_ScoreSubsystem)] → [Add Score]
```

Или для `WorldSubsystem`:

```
[Get World] → [Get Subsystem (UFP_WaveSubsystem)] → [Start Next Wave]
```

### Статическая Blueprint-функция для удобства

```cpp
// В любом BlueprintFunctionLibrary
UFUNCTION(BlueprintPure, Category = "Subsystems", meta = (WorldContext = "WorldContextObject"))
static UFP_WaveSubsystem* GetWaveSubsystem(const UObject* WorldContextObject)
{
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        return World->GetSubsystem<UFP_WaveSubsystem>();
    }
    return nullptr;
}
```

---

## Практические примеры

### Система сохранений (GameInstanceSubsystem)

```cpp
UCLASS()
class FPSGAME_API UFP_SaveSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable)
    void QuickSave();

    UFUNCTION(BlueprintCallable)
    void QuickLoad();

    UFUNCTION(BlueprintCallable)
    void SaveToSlot(const FString& SlotName, int32 UserIndex = 0);

    UFUNCTION(BlueprintCallable)
    bool LoadFromSlot(const FString& SlotName, int32 UserIndex = 0);

    UFUNCTION(BlueprintPure)
    TArray<FString> GetAllSaveSlots() const;

private:
    UPROPERTY()
    USaveGame* CurrentSaveGame = nullptr;

    FString QuickSaveSlotName = TEXT("QuickSave");
};
```

```cpp
void UFP_SaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Попробовать загрузить автосохранение
    if (UGameplayStatics::DoesSaveGameExist(QuickSaveSlotName, 0))
    {
        CurrentSaveGame = UGameplayStatics::LoadGameFromSlot(QuickSaveSlotName, 0);
    }
}

void UFP_SaveSubsystem::QuickSave()
{
    SaveToSlot(QuickSaveSlotName);
}

void UFP_SaveSubsystem::SaveToSlot(const FString& SlotName, int32 UserIndex)
{
    if (!CurrentSaveGame)
    {
        CurrentSaveGame = UGameplayStatics::CreateSaveGameObject(USaveGame::StaticClass());
    }
    UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, UserIndex);
}

bool UFP_SaveSubsystem::LoadFromSlot(const FString& SlotName, int32 UserIndex)
{
    if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex)) return false;

    CurrentSaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
    return CurrentSaveGame != nullptr;
}
```

---

### Система объективов/квестов (WorldSubsystem)

```cpp
USTRUCT(BlueprintType)
struct FObjectiveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ObjectiveID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredCount = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 CurrentCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bCompleted = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveCompleted, FName, ObjectiveID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveProgress, FName, ObjectiveID, float, Progress);

UCLASS()
class FPSGAME_API UFP_ObjectiveSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    UFUNCTION(BlueprintCallable, Category = "Objectives")
    void RegisterObjective(FObjectiveData Objective);

    UFUNCTION(BlueprintCallable, Category = "Objectives")
    void UpdateProgress(FName ObjectiveID, int32 Amount = 1);

    UFUNCTION(BlueprintPure, Category = "Objectives")
    bool IsObjectiveComplete(FName ObjectiveID) const;

    UFUNCTION(BlueprintPure, Category = "Objectives")
    float GetObjectiveProgress(FName ObjectiveID) const;

    UPROPERTY(BlueprintAssignable)
    FOnObjectiveCompleted OnObjectiveCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnObjectiveProgress OnObjectiveProgress;

private:
    UPROPERTY()
    TMap<FName, FObjectiveData> Objectives;
};
```

```cpp
void UFP_ObjectiveSubsystem::UpdateProgress(FName ObjectiveID, int32 Amount)
{
    FObjectiveData* Obj = Objectives.Find(ObjectiveID);
    if (!Obj || Obj->bCompleted) return;

    Obj->CurrentCount = FMath::Min(Obj->CurrentCount + Amount, Obj->RequiredCount);

    float Progress = static_cast<float>(Obj->CurrentCount) / Obj->RequiredCount;
    OnObjectiveProgress.Broadcast(ObjectiveID, Progress);

    if (Obj->CurrentCount >= Obj->RequiredCount)
    {
        Obj->bCompleted = true;
        OnObjectiveCompleted.Broadcast(ObjectiveID);
    }
}
```

---

### Система настроек игрока (LocalPlayerSubsystem)

```cpp
UCLASS()
class FPSGAME_API UFP_PlayerPrefsSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Preferences")
    void SetSensitivity(float Value);

    UFUNCTION(BlueprintPure, Category = "Preferences")
    float GetSensitivity() const { return Sensitivity; }

    UFUNCTION(BlueprintCallable, Category = "Preferences")
    void SetInvertY(bool bInvert);

    UFUNCTION(BlueprintPure, Category = "Preferences")
    bool IsYInverted() const { return bInvertY; }

    UFUNCTION(BlueprintCallable, Category = "Preferences")
    void SetFOV(float NewFOV);

    UFUNCTION(BlueprintPure, Category = "Preferences")
    float GetFOV() const { return FieldOfView; }

    // Применяет все настройки к контроллеру
    UFUNCTION(BlueprintCallable, Category = "Preferences")
    void ApplySettings();

private:
    float Sensitivity = 1.0f;
    bool bInvertY = false;
    float FieldOfView = 90.0f;
};
```

```cpp
void UFP_PlayerPrefsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Загрузить из конфига/сохранения
    // GConfig->GetFloat(TEXT("PlayerPrefs"), TEXT("Sensitivity"), Sensitivity, GGameIni);
}

void UFP_PlayerPrefsSubsystem::SetSensitivity(float Value)
{
    Sensitivity = FMath::Clamp(Value, 0.1f, 10.0f);
}

void UFP_PlayerPrefsSubsystem::SetInvertY(bool bInvert)
{
    bInvertY = bInvert;
}

void UFP_PlayerPrefsSubsystem::SetFOV(float NewFOV)
{
    FieldOfView = FMath::Clamp(NewFOV, 60.0f, 120.0f);
}

void UFP_PlayerPrefsSubsystem::ApplySettings()
{
    ULocalPlayer* LP = GetLocalPlayer();
    if (!LP) return;

    APlayerController* PC = LP->GetPlayerController(GetWorld());
    if (!PC) return;

    // Применить FOV
    if (APlayerCameraManager* CamMgr = PC->PlayerCameraManager)
    {
        CamMgr->SetFOV(FieldOfView);
    }
}
```

---

### Пул объектов (WorldSubsystem)

```cpp
UCLASS()
class FPSGAME_API UFP_PoolSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    // Предварительно создать N объектов
    UFUNCTION(BlueprintCallable, Category = "Pool")
    void WarmPool(TSubclassOf<AActor> ActorClass, int32 Count);

    // Получить объект из пула (или создать новый)
    UFUNCTION(BlueprintCallable, Category = "Pool")
    AActor* Acquire(TSubclassOf<AActor> ActorClass, const FTransform& Transform);

    // Вернуть объект в пул
    UFUNCTION(BlueprintCallable, Category = "Pool")
    void Release(AActor* Actor);

    // Статистика
    UFUNCTION(BlueprintPure, Category = "Pool")
    int32 GetPoolSize(TSubclassOf<AActor> ActorClass) const;

private:
    AActor* SpawnPooledActor(TSubclassOf<AActor> ActorClass);
    void DeactivateActor(AActor* Actor);
    void ActivateActor(AActor* Actor, const FTransform& Transform);

    // Ключ: UClass*, Значение: массив неактивных акторов
    TMap<UClass*, TArray<TWeakObjectPtr<AActor>>> InactivePool;

    // Все акторы (для очистки)
    TArray<TWeakObjectPtr<AActor>> AllPooledActors;
};
```

```cpp
void UFP_PoolSubsystem::WarmPool(TSubclassOf<AActor> ActorClass, int32 Count)
{
    if (!ActorClass) return;

    TArray<TWeakObjectPtr<AActor>>& ClassPool = InactivePool.FindOrAdd(ActorClass);

    for (int32 i = 0; i < Count; ++i)
    {
        if (AActor* Actor = SpawnPooledActor(ActorClass))
        {
            DeactivateActor(Actor);
            ClassPool.Add(Actor);
            AllPooledActors.Add(Actor);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Warmed pool for %s: %d actors"), *ActorClass->GetName(), Count);
}

AActor* UFP_PoolSubsystem::Acquire(TSubclassOf<AActor> ActorClass, const FTransform& Transform)
{
    if (!ActorClass) return nullptr;

    TArray<TWeakObjectPtr<AActor>>& ClassPool = InactivePool.FindOrAdd(ActorClass);

    // Поиск свободного
    while (ClassPool.Num() > 0)
    {
        TWeakObjectPtr<AActor> Weak = ClassPool.Pop();
        if (AActor* Actor = Weak.Get())
        {
            ActivateActor(Actor, Transform);
            return Actor;
        }
        // Если невалидный — пропускаем
    }

    // Пул пуст — создаём нового
    AActor* NewActor = SpawnPooledActor(ActorClass);
    if (NewActor)
    {
        ActivateActor(NewActor, Transform);
        AllPooledActors.Add(NewActor);
    }
    return NewActor;
}

void UFP_PoolSubsystem::Release(AActor* Actor)
{
    if (!IsValid(Actor)) return;

    DeactivateActor(Actor);
    TArray<TWeakObjectPtr<AActor>>& ClassPool = InactivePool.FindOrAdd(Actor->GetClass());
    ClassPool.Add(Actor);
}

AActor* UFP_PoolSubsystem::SpawnPooledActor(TSubclassOf<AActor> ActorClass)
{
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    return GetWorld()->SpawnActor<AActor>(ActorClass, FTransform::Identity, Params);
}

void UFP_PoolSubsystem::DeactivateActor(AActor* Actor)
{
    Actor->SetActorHiddenInGame(true);
    Actor->SetActorEnableCollision(false);
    Actor->SetActorTickEnabled(false);
}

void UFP_PoolSubsystem::ActivateActor(AActor* Actor, const FTransform& Transform)
{
    Actor->SetActorTransform(Transform);
    Actor->SetActorHiddenInGame(false);
    Actor->SetActorEnableCollision(true);
    Actor->SetActorTickEnabled(true);
}
```

---

### Система аудио-окружения (WorldSubsystem)

```cpp
UCLASS()
class FPSGAME_API UFP_AmbientAudioSubsystem : public UWorldSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual bool IsTickable() const override { return !IsTemplate(); }

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetAmbientZone(FName ZoneName, USoundBase* Sound, float FadeTime = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetIntensity(float NewIntensity);

private:
    UPROPERTY()
    UAudioComponent* ActiveAmbientComponent = nullptr;

    FName CurrentZone;
    float TargetIntensity = 1.0f;
    float CurrentIntensity = 1.0f;
};
```

---

## Подсистемы vs Singleton vs GameMode vs Manager

| Критерий | Подсистема | Синглтон (static) | GameMode | Manager Actor |
|---|---|---|---|---|
| Управление жизненным циклом | ✅ Автоматически | ❌ Ручное | ✅ Автоматически | ❌ Ручное |
| Доступ из Blueprint | ✅ Легко | ❌ Сложно | ✅ Легко | ✅ Через ссылку |
| Привязка к World | ✅ WorldSubsystem | ❌ Глобальный | ✅ | ❌ Надо управлять |
| Переживает смену уровня | ✅ GameInstanceSubsystem | ✅ | ❌ | ❌ |
| Работает с GC | ✅ UObject | ❌ | ✅ | ✅ |
| Тестируемость | ✅ Легко мокать | ❌ Тяжело | ⚠️ Средне | ⚠️ Средне |
| Порядок инициализации | ✅ InitializeDependency | ❌ Неопределён | ❌ | ❌ |
| Нужен Spawn/Placement | ❌ Не нужен | ❌ Не нужен | ❌ Не нужен | ✅ Нужен |
| Масштабируемость | ✅ Модульно | ❌ | ❌ Один GameMode | ⚠️ |

### Когда НЕ использовать подсистему

- Нужно несколько экземпляров → используй обычный `UObject` / `AActor`
- Нужна репликация по сети → используй `AActor` / `AInfo`
- Логика привязана к конкретному актору → используй `UActorComponent`
- Нужен доступ к Tick из коробки без интерфейса → используй `AActor`

---

## Взаимодействие подсистем между собой

### Через InitializeDependency

```cpp
void UFP_WaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Гарантирует порядок: SpawnSubsystem → WaveSubsystem
    Collection.InitializeDependency<UFP_SpawnSubsystem>();

    // Безопасно обращаться
    SpawnSubsystem = GetWorld()->GetSubsystem<UFP_SpawnSubsystem>();
}
```

### Через делегаты (рекомендуемый для слабой связанности)

```cpp
// ScoreSubsystem.cpp
void UFP_ScoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Подписка на событие волны
    if (UFP_WaveSubsystem* WaveSys = GetWorld()->GetSubsystem<UFP_WaveSubsystem>())
    {
        WaveSys->OnWaveCompleted.AddDynamic(this, &UFP_ScoreSubsystem::OnWaveCompleted);
    }
}

void UFP_ScoreSubsystem::Deinitialize()
{
    // Отписка
    if (UFP_WaveSubsystem* WaveSys = GetWorld()->GetSubsystem<UFP_WaveSubsystem>())
    {
        WaveSys->OnWaveCompleted.RemoveDynamic(this, &UFP_ScoreSubsystem::OnWaveCompleted);
    }
    Super::Deinitialize();
}

void UFP_ScoreSubsystem::OnWaveCompleted(int32 WaveNumber)
{
    AddScore(WaveNumber * 100);
}
```

### Между разными типами подсистем

```cpp
// WorldSubsystem обращается к GameInstanceSubsystem
void UFP_WaveSubsystem::OnAllWavesCompleted()
{
    // Сохранить прогресс через GameInstance подсистему (живёт между уровнями)
    UGameInstance* GI = GetWorld()->GetGameInstance();
    if (UFP_SaveSubsystem* SaveSys = GI->GetSubsystem<UFP_SaveSubsystem>())
    {
        SaveSys->QuickSave();
    }
}
```

---

## Подсистемы и сеть (Multiplayer)

> ⚠️ Подсистемы — это `UObject`, а не `AActor`. Они **НЕ реплицируются** по сети!

### Проблема

```
Сервер: UFP_WaveSubsystem → CurrentWave = 5
Клиент: UFP_WaveSubsystem → CurrentWave = 0  ← не синхронизирован!
```

### Решения

**1. Подсистема только на сервере, данные реплицируются через GameState:**

```cpp
// GameState реплицирует данные
UCLASS()
class AFP_GameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    UPROPERTY(ReplicatedUsing = OnRep_CurrentWave)
    int32 CurrentWave = 0;

    UFUNCTION()
    void OnRep_CurrentWave();
};

// Подсистема работает только на сервере
void UFP_WaveSubsystem::StartNextWave()
{
    if (!GetWorld()->IsServer()) return; // Только на сервере

    CurrentWave++;

    // Обновляем GameState для репликации
    if (AFP_GameState* GS = GetWorld()->GetGameState<AFP_GameState>())
    {
        GS->CurrentWave = CurrentWave;
    }
}
```

**2. Подсистема как обёртка над реплицированным актором:**

```cpp
void UFP_WaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Ищем реплицированного менеджера-актора
    // Подсистема — удобный интерфейс доступа, актор — транспорт данных
}
```

**3. Проверка сети в ShouldCreateSubsystem:**

```cpp
bool UFP_WaveSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (const UWorld* World = Cast<UWorld>(Outer))
    {
        // Создавать только на сервере / standalone
        return World->GetNetMode() != NM_Client;
    }
    return false;
}
```

---

## Антипаттерны и ошибки

### ❌ Хранить указатели на акторов без проверки

```cpp
// ПЛОХО — актор может быть уничтожен
AActor* CachedTarget;

void DoSomething()
{
    CachedTarget->DoStuff(); // КРАШ если актор уже уничтожен
}

// ХОРОШО — используй TWeakObjectPtr или проверяй IsValid
TWeakObjectPtr<AActor> CachedTarget;

void DoSomething()
{
    if (AActor* Target = CachedTarget.Get())
    {
        Target->DoStuff();
    }
}
```

### ❌ Обращаться к World в конструкторе

```cpp
// ПЛОХО — World ещё не существует в конструкторе подсистемы
UFP_MySubsystem::UFP_MySubsystem()
{
    GetWorld()->GetTimerManager(); // КРАШ!
}

// ХОРОШО — используй Initialize
void UFP_MySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    GetWorld()->GetTimerManager().SetTimer(...);
}
```

### ❌ Циклические зависимости

```cpp
// SubsystemA
Collection.InitializeDependency<USubsystemB>();

// SubsystemB
Collection.InitializeDependency<USubsystemA>(); // КРАШ! Циклическая зависимость!
```

### ❌ Забывать отписываться от делегатов

```cpp
// ПЛОХО — утечка подписки
void UFP_ScoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    WaveSys->OnWaveCompleted.AddDynamic(this, &UFP_ScoreSubsystem::OnWaveCompleted);
}
// Забыли Deinitialize → делегат содержит мёртвый указатель

// ХОРОШО — всегда отписывайся
void UFP_ScoreSubsystem::Deinitialize()
{
    if (UFP_WaveSubsystem* WaveSys = GetWorld()->GetSubsystem<UFP_WaveSubsystem>())
    {
        WaveSys->OnWaveCompleted.RemoveDynamic(this, &UFP_ScoreSubsystem::OnWaveCompleted);
    }
    Super::Deinitialize();
}
```

### ❌ Использовать подсистему как бога-объект

```cpp
// ПЛОХО — одна подсистема делает всё
class UGameManagerSubsystem : public UWorldSubsystem
{
    void SpawnEnemies();
    void UpdateScore();
    void SaveGame();
    void PlayMusic();
    void ManageUI();
    // 2000 строк кода...
};

// ХОРОШО — разделяй ответственность
class UFP_SpawnSubsystem : public UWorldSubsystem { ... };
class UFP_ScoreSubsystem : public UGameInstanceSubsystem { ... };
class UFP_SaveSubsystem  : public UGameInstanceSubsystem { ... };
class UFP_AudioSubsystem  : public UWorldSubsystem { ... };
```

### ❌ Пытаться реплицировать подсистему

```cpp
// ПЛОХО — UPROPERTY(Replicated) не работает в подсистемах
UPROPERTY(Replicated)
int32 Score; // Не будет реплицироваться! Это UObject, а не AActor.
```

---

## Шпаргалка — когда какой тип использовать

```
Нужна система, которая...

├── ...живёт весь процесс (аналитика, логи)
│   └── UEngineSubsystem
│
├── ...живёт между уровнями (сохранения, настройки, достижения)
│   └── UGameInstanceSubsystem
│
├── ...привязана к текущему миру (спавн, пул, волны, квесты)
│   └── UWorldSubsystem
│
├── ...уникальна для каждого игрока (настройки управления, UI)
│   └── ULocalPlayerSubsystem
│
├── ...работает только в редакторе (инструменты, валидация)
│   └── UEditorSubsystem
│
├── ...нужна репликация по сети
│   └── ❌ НЕ подсистема → используй AActor / AInfo
│
├── ...нужно несколько экземпляров
│   └── ❌ НЕ подсистема → используй UObject / AActor
│
└── ...привязана к конкретному актору
    └── ❌ НЕ подсистема → используй UActorComponent
```

---

## Шпаргалка — полный API

### Базовый класс USubsystem

```cpp
// Переопределяемые методы
virtual void Initialize(FSubsystemCollectionBase& Collection);  // Инициализация
virtual void Deinitialize();                                     // Очистка
virtual bool ShouldCreateSubsystem(UObject* Outer) const;       // Условие создания
```

### Получение подсистем

```cpp
// Engine
GEngine->GetEngineSubsystem<UMySubsystem>();

// Editor (только в редакторе)
GEditor->GetEditorSubsystem<UMySubsystem>();

// GameInstance
GetGameInstance()->GetSubsystem<UMySubsystem>();
// или
UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMySubsystem>();

// World
GetWorld()->GetSubsystem<UMySubsystem>();

// LocalPlayer
PC->GetLocalPlayer()->GetSubsystem<UMySubsystem>();
// или
ULocalPlayer::GetSubsystem<UMySubsystem>(LocalPlayer);
```

### Зависимости (в Initialize)

```cpp
Collection.InitializeDependency<UOtherSubsystem>();  // Гарантирует порядок
```

### Tick (через FTickableGameObject)

```cpp
virtual void Tick(float DeltaTime) override;
virtual TStatId GetStatId() const override;
virtual bool IsTickable() const override;
virtual bool IsTickableWhenPaused() const override;
virtual bool IsTickableInEditor() const override;
virtual UWorld* GetTickableGameObjectWorld() const override;
```

### Минимальный шаблон

```cpp
// MySubsystem.h
#pragma once
#include "Subsystems/WorldSubsystem.h"    // Или GameInstanceSubsystem.h и т.д.
#include "MySubsystem.generated.h"

UCLASS()
class FPSGAME_API UMySubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
};

// MySubsystem.cpp
#include "MySubsystem.h"

void UMySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UMySubsystem::Deinitialize()
{
    Super::Deinitialize();
}

bool UMySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (const UWorld* World = Cast<UWorld>(Outer))
    {
        return World->IsGameWorld();
    }
    return false;
}
```

### Необходимые инклуды

```cpp
#include "Subsystems/EngineSubsystem.h"           // UEngineSubsystem
#include "Subsystems/EditorSubsystem.h"            // UEditorSubsystem
#include "Subsystems/GameInstanceSubsystem.h"      // UGameInstanceSubsystem
#include "Subsystems/WorldSubsystem.h"             // UWorldSubsystem
#include "Subsystems/LocalPlayerSubsystem.h"       // ULocalPlayerSubsystem
#include "Tickable.h"                              // FTickableGameObject (для Tick)
```

