# UWorld — Все методы Trace, Sweep и Overlap

## Оглавление
- [Line Trace (Луч)](#line-trace)
- [Sweep (Форма по линии)](#sweep)
- [Overlap (Проверка в точке)](#overlap)
- [UKismetSystemLibrary — Blueprint-обёртки](#ukismetsystemlibrary)
- [Формы коллизий](#формы-коллизий)
- [Типы фильтрации](#типы-фильтрации)
- [FHitResult — полезные поля](#fhitresult)
- [FCollisionQueryParams](#fcollisionqueryparams)

---

## Line Trace

Лучевой каст — бесконечно тонкая линия от точки A до точки B.

| Метод | Описание |
|---|---|
| `LineTraceSingleByChannel` | Один хит, фильтр по **каналу** (Visibility, Camera и т.д.) |
| `LineTraceSingleByObjectType` | Один хит, фильтр по **типу объекта** (Pawn, WorldStatic и т.д.) |
| `LineTraceSingleByProfile` | Один хит, фильтр по **Collision Profile** (имя из настроек проекта) |
| `LineTraceMultiByChannel` | Все хиты (пробивает насквозь), фильтр по каналу |
| `LineTraceMultiByObjectType` | Все хиты, фильтр по типу объекта |
| `LineTraceMultiByProfile` | Все хиты, фильтр по профилю |
| `LineTraceTestByChannel` | Только `bool` — было попадание или нет (без `FHitResult`) |
| `LineTraceTestByObjectType` | Только `bool`, фильтр по типу объекта |
| `LineTraceTestByProfile` | Только `bool`, фильтр по профилю |

### Сигнатура (пример)

```cpp
bool LineTraceSingleByChannel(
    FHitResult& OutHit,
    const FVector& Start,
    const FVector& End,
    ECollisionChannel TraceChannel,
    const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
    const FCollisionResponseParams& ResponseParams = FCollisionResponseParams::DefaultResponseParam
);
```

---

## Sweep

Каст **формы** (сфера, бокс, капсула) вдоль линии от Start до End.

| Метод | Описание |
|---|---|
| `SweepSingleByChannel` | Один хит, форма по каналу |
| `SweepSingleByObjectType` | Один хит, форма по типу объекта |
| `SweepSingleByProfile` | Один хит, форма по профилю |
| `SweepMultiByChannel` | Все хиты, форма по каналу |
| `SweepMultiByObjectType` | Все хиты, форма по типу объекта |
| `SweepMultiByProfile` | Все хиты, форма по профилю |
| `SweepTestByChannel` | Только `bool`, форма по каналу |
| `SweepTestByObjectType` | Только `bool`, форма по типу объекта |
| `SweepTestByProfile` | Только `bool`, форма по профилю |

### Сигнатура (пример)

```cpp
bool SweepSingleByChannel(
    FHitResult& OutHit,
    const FVector& Start,
    const FVector& End,
    const FQuat& Rot,
    ECollisionChannel TraceChannel,
    const FCollisionShape& CollisionShape,
    const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
    const FCollisionResponseParams& ResponseParams = FCollisionResponseParams::DefaultResponseParam
);
```

---

## Overlap

Проверка всех объектов **внутри формы** в конкретной точке (без движения).

| Метод | Описание |
|---|---|
| `OverlapMultiByChannel` | Все перекрытия, фильтр по каналу |
| `OverlapMultiByObjectType` | Все перекрытия, фильтр по типу объекта |
| `OverlapMultiByProfile` | Все перекрытия, фильтр по профилю |
| `OverlapBlockingTestByChannel` | Только `bool` — есть blocking хит или нет |
| `OverlapBlockingTestByProfile` | Только `bool`, по профилю |
| `OverlapAnyTestByChannel` | `bool` — есть overlap **или** blocking |
| `OverlapAnyTestByObjectType` | `bool`, по типу объекта |
| `OverlapAnyTestByProfile` | `bool`, по профилю |

### Сигнатура (пример)

```cpp
bool OverlapMultiByChannel(
    TArray<FOverlapResult>& OutOverlaps,
    const FVector& Pos,
    const FQuat& Rot,
    ECollisionChannel TraceChannel,
    const FCollisionShape& CollisionShape,
    const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
    const FCollisionResponseParams& ResponseParams = FCollisionResponseParams::DefaultResponseParam
);
```

---

## UKismetSystemLibrary

Blueprint-дружественные обёртки (также вызываются из C++). Автоматически рисуют **дебаг**.

### Line Trace
| Метод | Описание |
|---|---|
| `LineTraceSingle` | Один хит, по каналу, с дебагом |
| `LineTraceMulti` | Все хиты, по каналу, с дебагом |
| `LineTraceSingleForObjects` | Один хит, по массиву Object Types |
| `LineTraceMultiForObjects` | Все хиты, по массиву Object Types |
| `LineTraceSingleByProfile` | Один хит, по профилю |
| `LineTraceMultiByProfile` | Все хиты, по профилю |

### Sphere Trace
| Метод | Описание |
|---|---|
| `SphereTraceSingle` | Сфера, один хит, по каналу |
| `SphereTraceMulti` | Сфера, все хиты, по каналу |
| `SphereTraceSingleForObjects` | Сфера, один хит, по Object Types |
| `SphereTraceMultiForObjects` | Сфера, все хиты, по Object Types |
| `SphereTraceSingleByProfile` | Сфера, один хит, по профилю |
| `SphereTraceMultiByProfile` | Сфера, все хиты, по профилю |

### Box Trace
| Метод | Описание |
|---|---|
| `BoxTraceSingle` | Бокс, один хит, по каналу |
| `BoxTraceMulti` | Бокс, все хиты, по каналу |
| `BoxTraceSingleForObjects` | Бокс, один хит, по Object Types |
| `BoxTraceMultiForObjects` | Бокс, все хиты, по Object Types |
| `BoxTraceSingleByProfile` | Бокс, один хит, по профилю |
| `BoxTraceMultiByProfile` | Бокс, все хиты, по профилю |

### Capsule Trace
| Метод | Описание |
|---|---|
| `CapsuleTraceSingle` | Капсула, один хит, по каналу |
| `CapsuleTraceMulti` | Капсула, все хиты, по каналу |
| `CapsuleTraceSingleForObjects` | Капсула, один хит, по Object Types |
| `CapsuleTraceMultiForObjects` | Капсула, все хиты, по Object Types |
| `CapsuleTraceSingleByProfile` | Капсула, один хит, по профилю |
| `CapsuleTraceMultiByProfile` | Капсула, все хиты, по профилю |

### Overlap
| Метод | Описание |
|---|---|
| `SphereOverlapActors` | Все акторы в сфере |
| `SphereOverlapComponents` | Все компоненты в сфере |
| `BoxOverlapActors` | Все акторы в боксе |
| `BoxOverlapComponents` | Все компоненты в боксе |
| `CapsuleOverlapActors` | Все акторы в капсуле |
| `CapsuleOverlapComponents` | Все компоненты в капсуле |
| `ComponentOverlapActors` | Overlap по форме конкретного компонента |
| `ComponentOverlapComponents` | Overlap компонентов по форме компонента |

---

## Формы коллизий

`FCollisionShape` — используется в Sweep и Overlap методах UWorld.

```cpp
FCollisionShape::MakeSphere(float Radius)
FCollisionShape::MakeBox(FVector HalfExtent)
FCollisionShape::MakeCapsule(float Radius, float HalfHeight)
```

---

## Типы фильтрации

### По каналу (Channel)
```cpp
ECC_Visibility       // луч видимости
ECC_Camera            // камера
ECC_WorldStatic       // статические объекты мира
ECC_WorldDynamic      // динамические объекты
ECC_Pawn              // пешки (персонажи)
ECC_PhysicsBody       // физические тела
ECC_Vehicle           // транспорт
ECC_Destructible      // разрушаемые
ECC_GameTraceChannel1 // пользовательские каналы (1-18)
ECC_GameTraceChannel2
// ...
```

### По типу объекта (Object Type)
```cpp
FCollisionObjectQueryParams ObjectParams;
ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
```

### По профилю (Profile)
```cpp
// Имя профиля из Project Settings → Collision → Collision Profiles
FName("BlockAll")
FName("OverlapAll")
FName("Pawn")
FName("WeaponTrace") // пользовательский
```

---

## FHitResult

| Поле | Тип | Описание |
|---|---|---|
| `bBlockingHit` | `bool` | Было ли блокирующее попадание |
| `bStartPenetrating` | `bool` | Начали ли внутри объекта |
| `Distance` | `float` | Расстояние от Start до попадания |
| `Time` | `float` | Нормализованное расстояние (0–1) |
| `Location` | `FVector` | Позиция центра формы в момент попадания |
| `ImpactPoint` | `FVector` | Точная точка контакта на поверхности |
| `Normal` | `FVector` | Нормаль формы трейса в точке попадания |
| `ImpactNormal` | `FVector` | Нормаль поверхности в точке попадания |
| `PhysMaterial` | `TWeakObjectPtr` | Физический материал поверхности |
| `BoneName` | `FName` | Имя кости (для Skeletal Mesh) |
| `FaceIndex` | `int32` | Индекс треугольника |
| `GetActor()` | `AActor*` | Актор в который попали |
| `GetComponent()` | `UPrimitiveComponent*` | Компонент в который попали |

---

## FCollisionQueryParams

```cpp
FCollisionQueryParams Params;

// Имя трейса (для дебага)
Params.TraceTag = FName("MyTrace");

// Игнорировать актора
Params.AddIgnoredActor(this);

// Игнорировать несколько акторов
Params.AddIgnoredActors(TArray<AActor*>{Actor1, Actor2});

// Игнорировать компонент
Params.AddIgnoredComponent(SomeComponent);

// Трейсить через Complex Collision (точные треугольники)
Params.bTraceComplex = true;

// Вернуть Face Index (для mesh decals)
Params.bReturnFaceIndex = true;

// Вернуть физический материал
Params.bReturnPhysicalMaterial = true;
```

---

## Разница: Single vs Multi vs Test

| Суффикс | Возвращает | Когда использовать |
|---|---|---|
| `Single` | `bool` + `FHitResult` | Нужен первый хит (выстрел, проверка) |
| `Multi` | `bool` + `TArray<FHitResult>` | Нужны все хиты (пробивание, дробовик) |
| `Test` | Только `bool` | Нужно только да/нет (проверка проходимости) |

## Разница: ByChannel vs ByObjectType vs ByProfile

| Фильтр | Описание |
|---|---|
| `ByChannel` | Фильтрует по Trace Channel — использует Collision Response таблицу |
| `ByObjectType` | Фильтрует по Object Type цели — игнорирует Collision Response |
| `ByProfile` | Фильтрует по Collision Profile — полная настройка из Project Settings |

---

## Дебаг отрисовка

```cpp
// Линия
DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, Duration);

// Сфера
DrawDebugSphere(GetWorld(), Center, Radius, Segments, FColor::Green, false, Duration);

// Бокс
DrawDebugBox(GetWorld(), Center, HalfSize, FColor::Blue, false, Duration);

// Капсула
DrawDebugCapsule(GetWorld(), Center, HalfHeight, Radius, FQuat::Identity, FColor::Yellow, false, Duration);

// Точка
DrawDebugPoint(GetWorld(), Point, Size, FColor::White, false, Duration);
```

