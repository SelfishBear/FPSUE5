# Математика в геймдеве — Полный гайд

## Содержание
- [Системы координат](#системы-координат)
- [Векторы](#векторы)
  - [Что такое вектор](#что-такое-вектор)
  - [Базовые операции](#базовые-операции)
  - [Длина вектора (Magnitude)](#длина-вектора-magnitude)
  - [Нормализация](#нормализация)
  - [Скалярное произведение (Dot Product)](#скалярное-произведение-dot-product)
  - [Векторное произведение (Cross Product)](#векторное-произведение-cross-product)
  - [Проекция вектора](#проекция-вектора)
  - [Отражение вектора](#отражение-вектора)
  - [Расстояние между точками](#расстояние-между-точками)
  - [Направление от A к B](#направление-от-a-к-b)
- [Матрицы](#матрицы)
  - [Что такое матрица](#что-такое-матрица)
  - [Матрица трансформации](#матрица-трансформации)
  - [Умножение матриц](#умножение-матриц)
  - [FTransform в UE](#ftransform-в-ue)
- [Кватернионы и вращения](#кватернионы-и-вращения)
  - [Euler Angles (углы Эйлера)](#euler-angles-углы-эйлера)
  - [Gimbal Lock](#gimbal-lock)
  - [Кватернионы](#кватернионы)
  - [FRotator vs FQuat](#frotator-vs-fquat)
- [Тригонометрия](#тригонометрия)
  - [Sin, Cos, Tan](#sin-cos-tan)
  - [Atan2 — угол по координатам](#atan2--угол-по-координатам)
  - [Градусы и радианы](#градусы-и-радианы)
  - [Единичная окружность](#единичная-окружность)
  - [Движение по кругу](#движение-по-кругу)
  - [Волновое движение (синусоида)](#волновое-движение-синусоида)
- [Интерполяция](#интерполяция)
  - [Lerp (линейная)](#lerp-линейная)
  - [FInterpTo (плавное приближение)](#finterpto-плавное-приближение)
  - [Ease функции](#ease-функции)
  - [Кривые Безье](#кривые-безье)
  - [Сплайны](#сплайны)
- [Физика и движение](#физика-и-движение)
  - [Скорость и ускорение](#скорость-и-ускорение)
  - [Гравитация и прыжок](#гравитация-и-прыжок)
  - [Импульс и сила](#импульс-и-сила)
  - [Трение](#трение)
  - [Баллистика — полёт пули/снаряда](#баллистика--полёт-пулиснаряда)
- [Столкновения и пересечения](#столкновения-и-пересечения)
  - [AABB (Axis-Aligned Bounding Box)](#aabb-axis-aligned-bounding-box)
  - [Сфера vs Сфера](#сфера-vs-сфера)
  - [Луч vs Плоскость](#луч-vs-плоскость)
  - [Луч vs Сфера](#луч-vs-сфера)
  - [Point in Box / Sphere](#point-in-box--sphere)
- [Рандом и шум](#рандом-и-шум)
  - [FMath::RandRange](#fmathrandrange)
  - [Random Unit Vector](#random-unit-vector)
  - [Perlin Noise](#perlin-noise)
  - [Weighted Random — взвешенный рандом](#weighted-random--взвешенный-рандом)
- [Геймплейная математика](#геймплейная-математика)
  - [FOV и конус видимости](#fov-и-конус-видимости)
  - [Впереди или позади?](#впереди-или-позади)
  - [Слева или справа?](#слева-или-справа)
  - [Расчёт урона по расстоянию (Damage Falloff)](#расчёт-урона-по-расстоянию-damage-falloff)
  - [Разброс оружия (Weapon Spread)](#разброс-оружия-weapon-spread)
  - [Наведение на цель (Homing)](#наведение-на-цель-homing)
  - [Предсказание позиции (Lead Target)](#предсказание-позиции-lead-target)
  - [Dash / Рывок](#dash--рывок)
  - [Knock Back — отбрасывание](#knock-back--отбрасывание)
  - [Camera Shake / Recoil](#camera-shake--recoil)
  - [Формулы урона и левелинга](#формулы-урона-и-левелинга)
- [FMath — шпаргалка по функциям UE](#fmath--шпаргалка-по-функциям-ue)
- [Визуальная шпаргалка](#визуальная-шпаргалка)

---

## Системы координат

### Unreal Engine использует **левостороннюю** систему координат

```
        Z (Up)
        |
        |
        |_______ Y (Right)
       /
      /
     X (Forward)
```

| Ось | Направление |
|-----|-------------|
| **X** | Вперёд (Forward) |
| **Y** | Вправо (Right) |
| **Z** | Вверх (Up) |

> Это отличается от Blender / OpenGL (правосторонняя, Y — вверх). При импорте моделей это учитывается автоматически.

### Единицы измерения

| Unreal Unit | Реальный мир |
|-------------|-------------|
| 1 UU | 1 см |
| 100 UU | 1 м |
| 100000 UU | 1 км |

Персонаж ростом ~180 UU = 1.8 метра.

---

## Векторы

### Что такое вектор

Вектор — это **направление + величина** (или просто набор координат).

```
Точка:  позиция в пространстве — (100, 200, 0)
Вектор: направление + длина     — "вперёд на 500 единиц"
```

В UE оба представлены как `FVector`:

```cpp
FVector Position(100.0f, 200.0f, 0.0f);   // Позиция
FVector Direction(1.0f, 0.0f, 0.0f);      // Направление (вперёд по X)
FVector Velocity(300.0f, 0.0f, 50.0f);    // Скорость (вектор)
```

### Специальные векторы

```cpp
FVector::ZeroVector     // (0, 0, 0)
FVector::OneVector      // (1, 1, 1)
FVector::UpVector       // (0, 0, 1)
FVector::DownVector     // (0, 0, -1)
FVector::ForwardVector  // (1, 0, 0)
FVector::RightVector    // (0, 1, 0)
FVector::LeftVector     // (0, -1, 0)
FVector::BackwardVector // (-1, 0, 0)
```

---

### Базовые операции

#### Сложение — перемещение

```
A + B = поточечное сложение

  A = (3, 1, 0)
  B = (2, 4, 0)
A+B = (5, 5, 0)
```

```cpp
FVector Position(100, 0, 0);
FVector Offset(0, 50, 0);
FVector NewPos = Position + Offset; // (100, 50, 0)
```

**Применение:** сдвинуть объект, добавить скорость к позиции.

```cpp
// Движение: Позиция += Скорость * DeltaTime
SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
```

#### Вычитание — направление от A к B

```
B - A = вектор ИЗ A В B

  A = (1, 2, 0)
  B = (4, 6, 0)
B-A = (3, 4, 0)   ← направление от A к B
```

```cpp
FVector From = GetActorLocation();
FVector To = Enemy->GetActorLocation();
FVector Direction = To - From; // Вектор от меня к врагу
```

#### Умножение на скаляр — масштабирование

```
V * s = каждый компонент умножается на s

V = (3, 4, 0)
V * 2 = (6, 8, 0)    ← в 2 раза длиннее, то же направление
V * -1 = (-3, -4, 0) ← развернули направление
V * 0.5 = (1.5, 2, 0) ← в 2 раза короче
```

```cpp
FVector Forward = GetActorForwardVector();
FVector Far = Forward * 1000.0f;   // 1000 единиц вперёд
FVector Back = Forward * -500.0f;  // 500 единиц назад
```

---

### Длина вектора (Magnitude)

```
|V| = sqrt(X² + Y² + Z²)

V = (3, 4, 0)
|V| = sqrt(9 + 16 + 0) = sqrt(25) = 5
```

```cpp
FVector V(3.0f, 4.0f, 0.0f);

float Length = V.Size();         // 5.0 — точная длина (дорогой sqrt)
float LengthSq = V.SizeSquared(); // 25.0 — без sqrt (быстрее!)
float Length2D = V.Size2D();     // Длина только по X,Y (игнорирует Z)
```

> **Оптимизация:** Если нужно **сравнить** расстояния — используй `SizeSquared()`. Квадратный корень дорогой, а для сравнения он не нужен:

```cpp
// ❌ Медленно
if (V.Size() < 500.0f)

// ✅ Быстро (сравниваем квадраты)
if (V.SizeSquared() < 500.0f * 500.0f)  // или < 250000.0f
```

---

### Нормализация

Нормализация — привести вектор к **длине 1**, сохранив направление.

```
Normalize(V) = V / |V|

V = (3, 4, 0), |V| = 5
Norm = (3/5, 4/5, 0) = (0.6, 0.8, 0)
|Norm| = 1
```

**Зачем:** чтобы получить **чистое направление** без длины. Потом умножаешь на нужную длину.

```cpp
FVector ToEnemy = Enemy->GetActorLocation() - GetActorLocation();

// Просто направление
FVector Direction = ToEnemy.GetSafeNormal(); // Возвращает (0,0,0) если длина ~0

// Или разделить на длину и направление
float Distance;
FVector Dir;
ToEnemy.ToDirectionAndLength(Dir, Distance);
// Dir = нормализованное направление
// Distance = расстояние до врага
```

**Типичный паттерн — "ПРЯМО от актора":**

```cpp
// Направление "вперёд" камеры/актора уже нормализовано!
FVector Forward = GetActorForwardVector(); // Длина всегда 1

// Точка в 1000 единицах впереди
FVector TraceEnd = TraceStart + Forward * 1000.0f;

// Направление к врагу, нормализованное, на 500 единиц
FVector ToEnemy = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
FVector LaunchPoint = GetActorLocation() + ToEnemy * 500.0f;
```

**Как задать направление вектору:**

```
Направление = Нормализованный_вектор * Дистанция

TraceEnd = Start + Direction * Range
```

Это считается так:
1. `Direction` — единичный вектор (длина 1), указывающий КУДА
2. `Range` — на СКОЛЬКО далеко
3. `Start + Direction * Range` — конечная точка

```cpp
// Пример: стрельба прямо от дула
FVector Start = MuzzleLocation;
FVector Direction = GetActorForwardVector(); // длина 1, направление "прямо"
float Range = 10000.0f;

FVector End = Start + Direction * Range;
// End = точка в 10000 единицах впереди от дула
```

---

### Скалярное произведение (Dot Product)

**Самая полезная операция в геймдеве.**

```
A · B = Ax*Bx + Ay*By + Az*Bz
A · B = |A| * |B| * cos(θ)

Если A и B нормализованы (длина 1):
A · B = cos(θ)
```

| Значение (нормализованные) | Угол | Значение |
|---|---|---|
| `1.0` | 0° | Смотрят в одну сторону |
| `0.0` | 90° | Перпендикулярны |
| `-1.0` | 180° | Смотрят друг на друга |
| `> 0` | < 90° | Примерно одно направление |
| `< 0` | > 90° | Примерно противоположные |

```cpp
float Dot = FVector::DotProduct(A, B);

// Или через оператор |
float Dot = A | B;
```

#### Применение 1: Впереди или позади?

```cpp
FVector Forward = GetActorForwardVector();
FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

float Dot = FVector::DotProduct(Forward, ToTarget);

if (Dot > 0.0f)
{
    // Цель ВПЕРЕДИ
}
else
{
    // Цель ПОЗАДИ
}
```

#### Применение 2: Конус видимости (FOV check)

```cpp
FVector Forward = GetActorForwardVector();
FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

float Dot = FVector::DotProduct(Forward, ToTarget);

// cos(45°) ≈ 0.707
// Если Dot > 0.707, цель в пределах 45° от направления взгляда
float HalfFOVCos = FMath::Cos(FMath::DegreesToRadians(45.0f)); // 0.707

if (Dot > HalfFOVCos)
{
    // Цель в поле зрения (90° конус = 45° в каждую сторону)
}
```

#### Применение 3: Угол между векторами

```cpp
float AngleRad = FMath::Acos(FVector::DotProduct(A.GetSafeNormal(), B.GetSafeNormal()));
float AngleDeg = FMath::RadiansToDegrees(AngleRad);

// Или встроенная функция UE (сразу в градусах):
float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(A.GetSafeNormal() | B.GetSafeNormal()));
```

#### Применение 4: Освещение (Lambert)

```cpp
// Насколько поверхность освещена
FVector LightDir = (LightPos - SurfacePos).GetSafeNormal();
FVector Normal = SurfaceNormal;

float Intensity = FMath::Max(0.0f, FVector::DotProduct(Normal, LightDir));
// 1.0 = полностью освещена, 0.0 = в тени
```

---

### Векторное произведение (Cross Product)

Возвращает вектор, **перпендикулярный** обоим входным.

```
A × B = вектор, перпендикулярный A и B

Направление определяется правилом левой руки (в UE — левосторонняя система)
```

```cpp
FVector Cross = FVector::CrossProduct(A, B);

// Или через оператор ^
FVector Cross = A ^ B;
```

> **Важно:** `A × B ≠ B × A`. Результат меняет направление! `A × B = -(B × A)`

#### Применение 1: Слева или справа?

```cpp
FVector Forward = GetActorForwardVector();
FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

FVector Cross = FVector::CrossProduct(Forward, ToTarget);

if (Cross.Z > 0.0f)
{
    // Цель СПРАВА
}
else
{
    // Цель СЛЕВА
}
```

#### Применение 2: Нормаль поверхности

```cpp
// По трём точкам треугольника
FVector Edge1 = B - A;
FVector Edge2 = C - A;
FVector Normal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();
```

#### Комбинация Dot + Cross: полное определение позиции цели

```cpp
FVector Forward = GetActorForwardVector();
FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

float Dot = FVector::DotProduct(Forward, ToTarget);
FVector Cross = FVector::CrossProduct(Forward, ToTarget);

// Dot > 0 = впереди, < 0 = позади
// Cross.Z > 0 = справа, < 0 = слева

if (Dot > 0.7f)         UE_LOG(LogTemp, Log, TEXT("Прямо впереди"));
else if (Dot > 0.0f && Cross.Z > 0.0f)  UE_LOG(LogTemp, Log, TEXT("Впереди-справа"));
else if (Dot > 0.0f && Cross.Z < 0.0f)  UE_LOG(LogTemp, Log, TEXT("Впереди-слева"));
else if (Dot < 0.0f && Cross.Z > 0.0f)  UE_LOG(LogTemp, Log, TEXT("Позади-справа"));
else if (Dot < 0.0f && Cross.Z < 0.0f)  UE_LOG(LogTemp, Log, TEXT("Позади-слева"));
else                     UE_LOG(LogTemp, Log, TEXT("Прямо позади"));
```

---

### Проекция вектора

Проекция A на B — "тень" вектора A на направление B.

```
           A
          /|
         / |
        /  |  ← проекция A на B
       /   |
      ───────── B
      |proj|
```

```cpp
// Скалярная проекция (длина тени)
float ScalarProj = FVector::DotProduct(A, B.GetSafeNormal());

// Векторная проекция (вектор тени)
FVector Proj = A.ProjectOnTo(B);

// Проекция на нормаль (компонент вдоль нормали)
FVector ProjOnNormal = A.ProjectOnToNormal(Normal); // Normal должен быть единичным
```

**Применение: компонент скорости вдоль направления**

```cpp
FVector Velocity = GetCharacterMovement()->Velocity;
FVector Forward = GetActorForwardVector();

// Скорость "вперёд" (отрицательная = движемся назад)
float ForwardSpeed = FVector::DotProduct(Velocity, Forward);
```

---

### Отражение вектора

```
      N (нормаль)
      |
  V \ | / R (отражение)
     \|/
──────────── поверхность
```

```cpp
FVector Reflected = FMath::GetReflectionVector(Velocity, SurfaceNormal);

// Или через FVector
FVector Reflected = Velocity.MirrorByVector(SurfaceNormal);
```

**Применение:** рикошет пули, отскок мяча.

```cpp
FVector BounceVelocity = FMath::GetReflectionVector(BulletVelocity, HitResult.ImpactNormal);
```

---

### Расстояние между точками

```
Dist = |B - A| = sqrt((Bx-Ax)² + (By-Ay)² + (Bz-Az)²)
```

```cpp
float Dist = FVector::Dist(A, B);              // Полное 3D расстояние
float Dist2D = FVector::Dist2D(A, B);          // Только X,Y (игнорирует Z)
float DistSq = FVector::DistSquared(A, B);     // Без sqrt (для сравнений)
```

**Паттерн: проверка дальности без sqrt**

```cpp
float MaxRange = 1000.0f;

// ❌ Медленно
if (FVector::Dist(MyPos, EnemyPos) < MaxRange)

// ✅ Быстро
if (FVector::DistSquared(MyPos, EnemyPos) < MaxRange * MaxRange)
```

---

### Направление от A к B

Одна из самых частых задач:

```cpp
FVector A = GetActorLocation();
FVector B = Target->GetActorLocation();

// Направление (ненормализованное — содержит расстояние)
FVector Raw = B - A;

// Нормализованное направление (длина = 1)
FVector Dir = (B - A).GetSafeNormal();

// Направление + расстояние за один вызов
FVector Dir;
float Distance;
(B - A).ToDirectionAndLength(Dir, Distance);
```

---

## Матрицы

### Что такое матрица

Матрица — таблица чисел, используется для **трансформаций**: перемещение, вращение, масштабирование.

```
4x4 матрица трансформации:

| Rx  Ry  Rz  0 |   ← Вращение (строка 1)
| Ux  Uy  Uz  0 |   ← Вращение (строка 2)
| Fx  Fy  Fz  0 |   ← Вращение (строка 3)
| Tx  Ty  Tz  1 |   ← Перемещение (Translation)
```

### Матрица трансформации

В геймдеве матрица 4x4 объединяет:
- **Вращение** (3x3 подматрица)
- **Перемещение** (Translation, последняя строка)
- **Масштаб** (Scale, закодирован в длинах осей вращения)

```cpp
FMatrix Mat = FMatrix::Identity; // Единичная матрица (ничего не меняет)

// Получить из трансформа
FMatrix Mat = GetActorTransform().ToMatrixWithScale();

// Извлечь данные
FVector Origin = Mat.GetOrigin();         // Позиция
FVector Scale = Mat.GetScaleVector();     // Масштаб
FRotator Rot = Mat.Rotator();            // Вращение
```

### Умножение матриц

Умножение матриц = **комбинирование трансформаций**. Порядок важен!

```
M = Scale * Rotation * Translation

Применение: Point' = Point * M
```

```cpp
FMatrix A = ...; // Первая трансформация
FMatrix B = ...; // Вторая трансформация
FMatrix Combined = A * B; // Сначала A, потом B

// Трансформировать точку
FVector Transformed = A.TransformPosition(Point);

// Трансформировать направление (без Translation)
FVector TransformedDir = A.TransformVector(Direction);
```

### FTransform в UE

В повседневном коде ты редко работаешь с матрицами напрямую. UE предоставляет `FTransform`:

```cpp
FTransform Transform = GetActorTransform();

// Компоненты
FVector Location = Transform.GetLocation();
FQuat Rotation = Transform.GetRotation();
FVector Scale = Transform.GetScale3D();

// Трансформация точки из локального в мировое пространство
FVector WorldPos = Transform.TransformPosition(LocalPos);

// Обратно: из мирового в локальное
FVector LocalPos = Transform.InverseTransformPosition(WorldPos);

// Трансформация направления
FVector WorldDir = Transform.TransformVector(LocalDir);

// Комбинирование
FTransform Combined = ChildTransform * ParentTransform; // Ребёнок в мировом пространстве

// Инверсия (обратная трансформация)
FTransform Inverse = Transform.Inverse();
```

---

## Кватернионы и вращения

### Euler Angles (углы Эйлера)

Три угла: Pitch, Yaw, Roll.

```
Pitch (Y) — наклон вверх/вниз (кивок головой)
Yaw   (Z) — поворот влево/вправо (мотание головой)  
Roll  (X) — наклон вбок (ухо к плечу)
```

```cpp
FRotator Rot(Pitch, Yaw, Roll);
FRotator Rot(0.0f, 90.0f, 0.0f); // Повёрнут на 90° вправо

// Получить
FRotator Rot = GetActorRotation();
float Yaw = Rot.Yaw;

// Получить направление из ротатора
FVector Forward = Rot.Vector(); // Единичный вектор "вперёд"

// Ротатор из направления
FRotator Rot = Direction.Rotation();

// Повернуть на дельту
FRotator NewRot = CurrentRot + FRotator(0.0f, 45.0f, 0.0f); // +45° Yaw
```

### Gimbal Lock

Проблема углов Эйлера — **Gimbal Lock**. Когда два кольца вращения совпадают (Pitch = ±90°), теряется одна степень свободы. Объект "залипает" и не может повернуться как нужно.

**Решение:** кватернионы.

### Кватернионы

Кватернион — 4 числа `(X, Y, Z, W)`, представляющие вращение вокруг оси на угол.

```
Q = (sin(θ/2) * Axis, cos(θ/2))

Ось вращения: (Ax, Ay, Az) — единичный вектор
Угол: θ

Q.X = sin(θ/2) * Ax
Q.Y = sin(θ/2) * Ay
Q.Z = sin(θ/2) * Az
Q.W = cos(θ/2)
```

**Тебе НЕ нужно понимать математику кватернионов.** Нужно знать как их использовать:

```cpp
// Создание
FQuat Q = FQuat::Identity; // Без вращения

// Из оси + угла
FQuat Q = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0f)); // 90° вокруг Z

// Из FRotator
FQuat Q = FRotator(0, 90, 0).Quaternion();

// Обратно в FRotator
FRotator R = Q.Rotator();

// Вращение вектора кватернионом
FVector Rotated = Q.RotateVector(OriginalVector);

// Комбинирование вращений
FQuat Combined = Q1 * Q2; // Сначала Q1, потом Q2

// Интерполяция (плавный поворот)
FQuat Smooth = FQuat::Slerp(From, To, Alpha); // Alpha 0..1

// Обратный кватернион
FQuat Inverse = Q.Inverse();
```

### FRotator vs FQuat

| Критерий | FRotator | FQuat |
|----------|----------|-------|
| Читаемость | ✅ Понятные углы (Pitch, Yaw, Roll) | ❌ 4 абстрактных числа |
| Gimbal Lock | ❌ Есть | ✅ Нет |
| Интерполяция | ❌ Артефакты | ✅ Slerp — идеально |
| Производительность | Медленнее | Быстрее |
| Используй когда | Ввод игрока, UI, простые повороты | Анимации, плавные повороты, внутренние расчёты |

**Правило:** Принимай ввод как `FRotator`, внутри работай с `FQuat`, отдавай как `FRotator`.

---

## Тригонометрия

### Sin, Cos, Tan

```
В прямоугольном треугольнике:

         /|
    c   / |
(hyp)  /  | b (opposite)
      /θ__|
        a (adjacent)

sin(θ) = b / c  (противолежащий / гипотенуза)
cos(θ) = a / c  (прилежащий / гипотенуза)
tan(θ) = b / a  (противолежащий / прилежащий)
```

```cpp
// UE работает в РАДИАНАХ!
float Rad = FMath::DegreesToRadians(45.0f);

float S = FMath::Sin(Rad);  // 0.707
float C = FMath::Cos(Rad);  // 0.707
float T = FMath::Tan(Rad);  // 1.0

// SinCos — вычисляет оба за один вызов (быстрее)
float S, C;
FMath::SinCos(&S, &C, Rad);
```

### Atan2 — угол по координатам

`Atan2` возвращает угол от оси X к точке (X, Y). Работает для всех 360°.

```
      Y
      |  * (X,Y)
      | /
      |/ θ
------+-------- X
```

```cpp
float AngleRad = FMath::Atan2(Y, X);
float AngleDeg = FMath::RadiansToDegrees(AngleRad);

// Пример: угол к врагу в 2D (вид сверху)
FVector ToEnemy = Enemy->GetActorLocation() - GetActorLocation();
float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(ToEnemy.Y, ToEnemy.X));
```

### Градусы и радианы

```
360° = 2π радиан
180° = π радиан
90°  = π/2 радиан
1°   = π/180 ≈ 0.01745 радиан
1 рад = 180/π ≈ 57.296°
```

```cpp
float Rad = FMath::DegreesToRadians(90.0f);   // 1.5708
float Deg = FMath::RadiansToDegrees(PI);       // 180.0

// Константы
PI          // 3.14159265...
HALF_PI     // 1.5707963...  (90°)
TWO_PI      // 6.2831853...  (360°)
INV_PI      // 1/π
```

### Единичная окружность

```
        90° (0, 1)
         |
180°  ───+─── 0° (1, 0)
(-1,0)   |
        270° (0, -1)

Для любого угла θ:
  X = cos(θ)
  Y = sin(θ)
```

Это основа для **движения по кругу**, **размещения объектов по окружности**, **направлений**.

### Движение по кругу

```cpp
// Вращение объекта вокруг центра
void AMyActor::Tick(float DeltaTime)
{
    CurrentAngle += RotationSpeed * DeltaTime;

    float X = Center.X + Radius * FMath::Cos(CurrentAngle);
    float Y = Center.Y + Radius * FMath::Sin(CurrentAngle);

    SetActorLocation(FVector(X, Y, Center.Z));
}
```

#### Размещение объектов по кругу

```cpp
void SpawnInCircle(int32 Count, float Radius, FVector Center)
{
    float AngleStep = TWO_PI / Count; // Угол между объектами

    for (int32 i = 0; i < Count; i++)
    {
        float Angle = AngleStep * i;
        FVector Pos = Center + FVector(
            Radius * FMath::Cos(Angle),
            Radius * FMath::Sin(Angle),
            0.0f
        );
        GetWorld()->SpawnActor<AActor>(ActorClass, Pos, FRotator::ZeroRotator);
    }
}
// Count = 6, Radius = 500 → гексагон из 6 объектов
```

### Волновое движение (синусоида)

```cpp
// Покачивание вверх-вниз (hover)
void AMyActor::Tick(float DeltaTime)
{
    RunningTime += DeltaTime;

    float DeltaZ = Amplitude * FMath::Sin(Frequency * RunningTime);
    FVector NewLocation = InitialLocation;
    NewLocation.Z += DeltaZ;

    SetActorLocation(NewLocation);
}
// Amplitude = 50.0f → качается на ±50 единиц
// Frequency = 2.0f  → 2 колебания за 2π секунд
```

#### Пульсация масштаба

```cpp
float Scale = 1.0f + 0.2f * FMath::Sin(GetWorld()->GetTimeSeconds() * 3.0f);
SetActorScale3D(FVector(Scale));
// Пульсирует от 0.8 до 1.2
```

---

## Интерполяция

### Lerp (линейная)

**Lerp** (Linear Interpolation) — плавный переход от A к B.

```
Lerp(A, B, Alpha) = A + (B - A) * Alpha

Alpha = 0.0 → A
Alpha = 0.5 → середина
Alpha = 1.0 → B
```

```cpp
// Числа
float Result = FMath::Lerp(0.0f, 100.0f, 0.5f); // 50.0

// Векторы
FVector Pos = FMath::Lerp(StartPos, EndPos, Alpha);

// Вращения
FRotator Rot = FMath::Lerp(StartRot, EndRot, Alpha);

// Цвета
FLinearColor Color = FMath::Lerp(Red, Blue, Alpha);
```

#### Lerp в Tick (плавное движение)

```cpp
// Постоянная скорость — Alpha от 0 до 1
void AMyActor::Tick(float DeltaTime)
{
    Progress += DeltaTime * Speed; // Speed = 0.5 → за 2 сек
    Progress = FMath::Clamp(Progress, 0.0f, 1.0f);

    SetActorLocation(FMath::Lerp(StartPos, EndPos, Progress));
}
```

#### Lerp для "замедления" (Smooth Damp)

```cpp
// Каждый кадр Alpha = DeltaTime * Speed
// Быстро в начале, замедляется к концу (экспоненциальное приближение)
void AMyActor::Tick(float DeltaTime)
{
    FVector Current = GetActorLocation();
    FVector Target = TargetLocation;

    FVector NewPos = FMath::Lerp(Current, Target, DeltaTime * 5.0f);
    SetActorLocation(NewPos);
}
// Никогда точно не достигнет Target, но визуально приблизится
```

> ⚠️ `Lerp(Current, Target, DeltaTime * Speed)` — **НЕ** frame-rate independent!
> Для корректности используй `FInterpTo` или `FMath::Lerp(Current, Target, 1.0f - FMath::Exp(-Speed * DeltaTime))`.

---

### FInterpTo (плавное приближение)

**Frame-rate independent** плавное приближение. Используй вместо `Lerp(Current, Target, DT * Speed)`.

```cpp
// Float
float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, InterpSpeed);

// Vector
FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, DeltaTime, InterpSpeed);

// Rotator
FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, InterpSpeed);

// Constant speed (не замедляется)
FVector NewPos = FMath::VInterpConstantTo(CurrentPos, TargetPos, DeltaTime, Speed);
FRotator NewRot = FMath::RInterpConstantTo(CurrentRot, TargetRot, DeltaTime, Speed);
```

| Функция | Поведение |
|---------|-----------|
| `FInterpTo` | Быстро → замедляется (ease-out) |
| `FInterpConstantTo` | Постоянная скорость |

```cpp
// Пример: плавное следование камеры
void AMyCamera::Tick(float DeltaTime)
{
    FVector TargetPos = FollowTarget->GetActorLocation();
    FVector NewPos = FMath::VInterpTo(GetActorLocation(), TargetPos, DeltaTime, 5.0f);
    SetActorLocation(NewPos);
}
```

---

### Ease функции

Нелинейная интерполяция — разная скорость в начале и конце.

```
EaseIn:     медленно → быстро      (ускорение)
EaseOut:    быстро → медленно      (торможение)
EaseInOut:  медленно → быстро → медленно

Линейная:   ──────────────
EaseIn:     ─────────╱
EaseOut:    ╱─────────
EaseInOut:  ───╱╲───
```

```cpp
// Через FMath::InterpEaseIn / Out / InOut
float Alpha = 0.5f; // 0..1
float Exp = 2.0f;   // Степень (2 = квадратичная, 3 = кубическая)

float EaseIn    = FMath::InterpEaseIn(0.0f, 1.0f, Alpha, Exp);
float EaseOut   = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, Exp);
float EaseInOut = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, Exp);

// Ручная реализация:
// EaseIn:    Alpha^Exp
// EaseOut:   1 - (1 - Alpha)^Exp
// EaseInOut: SmoothStep или Hermite
```

```cpp
// SmoothStep (встроенная в FMath)
float Smooth = FMath::SmoothStep(0.0f, 1.0f, Alpha);
// Формула: 3α² - 2α³ (плавный S-изгиб)
```

#### Spring Interp (пружина)

```cpp
float SpringResult = FMath::InterpSpringTo(
    Current, Target, SpringState, DeltaTime,
    Stiffness,  // жёсткость пружины
    DampRatio   // затухание (1.0 = без осцилляции)
);
```

---

### Кривые Безье

#### Квадратичная кривая Безье (3 точки)

```
P0 ──── P1
          \
           P2

B(t) = (1-t)²·P0 + 2(1-t)t·P1 + t²·P2
```

```cpp
FVector QuadraticBezier(FVector P0, FVector P1, FVector P2, float T)
{
    float U = 1.0f - T;
    return U * U * P0 + 2.0f * U * T * P1 + T * T * P2;
}

// Пример: дуга снаряда
for (float T = 0.0f; T <= 1.0f; T += 0.01f)
{
    FVector Point = QuadraticBezier(Start, ControlPoint, End, T);
    DrawDebugPoint(GetWorld(), Point, 5.0f, FColor::Red, false, 2.0f);
}
```

#### Кубическая кривая Безье (4 точки)

```
B(t) = (1-t)³·P0 + 3(1-t)²t·P1 + 3(1-t)t²·P2 + t³·P3
```

```cpp
FVector CubicBezier(FVector P0, FVector P1, FVector P2, FVector P3, float T)
{
    float U = 1.0f - T;
    return U*U*U*P0 + 3*U*U*T*P1 + 3*U*T*T*P2 + T*T*T*P3;
}
```

---

### Сплайны

UE имеет встроенные сплайн-компоненты:

```cpp
UPROPERTY(VisibleAnywhere)
TObjectPtr<USplineComponent> SplinePath;

// Получить точку на сплайне по дистанции
FVector Pos = SplinePath->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

// По проценту (0..1)
FVector Pos = SplinePath->GetLocationAtSplineInputKey(Alpha * SplinePath->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);

// Направление
FVector Dir = SplinePath->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

// Длина
float TotalLength = SplinePath->GetSplineLength();
```

---

## Физика и движение

### Скорость и ускорение

```
Скорость = Позиция' (производная позиции по времени)
Ускорение = Скорость' (производная скорости по времени)

Position += Velocity * DeltaTime
Velocity += Acceleration * DeltaTime
```

```cpp
// Простое движение
void AProjectile::Tick(float DeltaTime)
{
    Velocity += Acceleration * DeltaTime;
    FVector NewLocation = GetActorLocation() + Velocity * DeltaTime;
    SetActorLocation(NewLocation);
}
```

### Гравитация и прыжок

```
Гравитация = ускорение вниз

g ≈ 980 UU/s² (в UE — аналог 9.8 м/с²)

Velocity.Z += Gravity * DeltaTime    (отрицательное = вниз)
Position.Z += Velocity.Z * DeltaTime
```

```cpp
// Простой прыжок без CharacterMovement
void AMyActor::Jump()
{
    Velocity.Z = JumpForce; // Начальная скорость вверх (например 600)
}

void AMyActor::Tick(float DeltaTime)
{
    if (!bOnGround)
    {
        Velocity.Z += Gravity * DeltaTime; // Gravity = -980
        FVector NewPos = GetActorLocation() + Velocity * DeltaTime;
        SetActorLocation(NewPos);
    }
}
```

**Высота прыжка по начальной скорости:**

```
h = V₀² / (2 * |g|)

V₀ = 600, g = 980
h = 360000 / 1960 ≈ 183.67 UU ≈ 1.84м
```

**Начальная скорость по нужной высоте:**

```
V₀ = sqrt(2 * |g| * h)

h = 200 UU, g = 980
V₀ = sqrt(2 * 980 * 200) = sqrt(392000) ≈ 626
```

### Импульс и сила

```
Сила (Force):     ускорение, действует постоянно, F = m * a
Импульс (Impulse): мгновенное изменение скорости, ΔV = Impulse / mass
```

```cpp
// Импульс (мгновенный толчок)
MeshComponent->AddImpulse(Direction * ImpulseStrength);

// Сила (постоянное воздействие, вызывай каждый кадр)
MeshComponent->AddForce(Direction * ForceStrength);

// CharacterMovement
GetCharacterMovement()->AddImpulse(FVector(0, 0, 50000)); // Подбросить
GetCharacterMovement()->AddForce(FVector(1000, 0, 0));     // Толкать вперёд
```

### Трение

```
Vновая = V * (1 - Friction * DeltaTime)

Или экспоненциальное:
Vновая = V * FMath::Exp(-Friction * DeltaTime)
```

```cpp
// Простое трение
Velocity *= (1.0f - Friction * DeltaTime);

// Экспоненциальное (frame-rate independent)
Velocity *= FMath::Exp(-Friction * DeltaTime);

// Если скорость почти нулевая — обнулить
if (Velocity.SizeSquared() < 1.0f)
{
    Velocity = FVector::ZeroVector;
}
```

### Баллистика — полёт пули/снаряда

#### Hitscan (мгновенный луч)

```cpp
FVector Start = MuzzleLocation;
FVector End = Start + ShotDirection * MaxRange;

FHitResult Hit;
if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
{
    // Попал
}
```

#### Projectile (физический снаряд)

```cpp
void AProjectile::Tick(float DeltaTime)
{
    // Гравитация
    Velocity += FVector(0, 0, Gravity) * DeltaTime;

    FVector Start = GetActorLocation();
    FVector End = Start + Velocity * DeltaTime;

    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
    {
        OnHit(Hit);
        Destroy();
    }
    else
    {
        SetActorLocation(End);
        SetActorRotation(Velocity.Rotation()); // Поворот по направлению
    }
}
```

#### Параболическая траектория (предсказание)

```cpp
// Предсказать где упадёт снаряд
TArray<FVector> PredictTrajectory(FVector Start, FVector Velocity, float Gravity, float TimeStep, int32 Steps)
{
    TArray<FVector> Points;
    FVector Pos = Start;
    FVector Vel = Velocity;

    for (int32 i = 0; i < Steps; i++)
    {
        Points.Add(Pos);
        Vel.Z += Gravity * TimeStep;
        Pos += Vel * TimeStep;
    }
    return Points;
}
```

---

## Столкновения и пересечения

### AABB (Axis-Aligned Bounding Box)

Прямоугольник, выровненный по осям. Самая быстрая проверка.

```
Box A: Min(0,0), Max(2,2)
Box B: Min(1,1), Max(3,3)

Пересекаются если:
  A.Max.X > B.Min.X && A.Min.X < B.Max.X &&
  A.Max.Y > B.Min.Y && A.Min.Y < B.Max.Y &&
  A.Max.Z > B.Min.Z && A.Min.Z < B.Max.Z
```

```cpp
FBox BoxA(FVector(0), FVector(200));
FBox BoxB(FVector(100), FVector(300));

bool bOverlap = BoxA.Intersect(BoxB); // true

// Или через FVector
bool bInside = BoxA.IsInside(Point); // Точка внутри бокса?
```

### Сфера vs Сфера

```
Пересекаются если:
  Distance(CenterA, CenterB) < RadiusA + RadiusB

Оптимизированно (без sqrt):
  DistSquared < (RadiusA + RadiusB)²
```

```cpp
float DistSq = FVector::DistSquared(SphereA.Center, SphereB.Center);
float RadiusSum = SphereA.Radius + SphereB.Radius;
bool bOverlap = DistSq < RadiusSum * RadiusSum;
```

### Луч vs Плоскость

```
Плоскость: точка P₀ и нормаль N
Луч: начало O, направление D

t = dot(P₀ - O, N) / dot(D, N)
Точка пересечения = O + D * t

Если dot(D, N) ≈ 0 → луч параллелен плоскости
Если t < 0 → пересечение позади луча
```

```cpp
FPlane Plane(PlanePoint, PlaneNormal);
FVector Intersection;
float T;
bool bHit = FMath::SegmentPlaneIntersection(Start, End, Plane, Intersection);
```

### Луч vs Сфера

```
Луч: O + D*t
Сфера: центр C, радиус R

L = C - O
tca = dot(L, D)
d² = dot(L, L) - tca²
Если d² > R² → промах
```

```cpp
// UE делает это за тебя через SweepSingle / LineTrace
FHitResult Hit;
FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Visibility, Sphere);
```

### Point in Box / Sphere

```cpp
// Точка внутри бокса
FBox Box(Min, Max);
bool bInBox = Box.IsInside(Point);

// Точка внутри сферы
bool bInSphere = FVector::DistSquared(Center, Point) <= Radius * Radius;

// UE FSphere
FSphere Sphere(Center, Radius);
bool bInside = Sphere.IsInside(Point);
```

---

## Рандом и шум

### FMath::RandRange

```cpp
// Int (включая оба конца)
int32 Roll = FMath::RandRange(1, 6);        // 1..6

// Float (включая оба конца)
float Spread = FMath::RandRange(-5.0f, 5.0f); // -5..5

// 0..1
float Random01 = FMath::FRand();             // 0.0 .. 1.0

// Bool с вероятностью
bool bCrit = FMath::FRand() < 0.25f;        // 25% шанс

// Или
bool bCrit = FMath::RandBool();              // 50/50
```

### Random Unit Vector

```cpp
// Случайное направление в 3D (на поверхности сферы)
FVector RandDir = FMath::VRand(); // Нормализованный

// Случайное направление в конусе
FVector ConeDir = FMath::VRandCone(
    ForwardVector,                              // Центр конуса
    FMath::DegreesToRadians(15.0f)              // Половина угла конуса
);

// Случайная точка в сфере
FVector RandInSphere = FMath::VRand() * FMath::FRandRange(0.0f, Radius);

// Случайная точка на окружности (2D)
float Angle = FMath::FRandRange(0.0f, TWO_PI);
FVector Point(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 0.0f);

// Случайная точка в круге (2D, равномерно)
float R = FMath::Sqrt(FMath::FRand()) * Radius; // sqrt для равномерности!
float A = FMath::FRandRange(0.0f, TWO_PI);
FVector Point(FMath::Cos(A) * R, FMath::Sin(A) * R, 0.0f);
```

### Perlin Noise

Плавный шум — для процедурной генерации, покачивания камеры, ландшафта.

```cpp
// 1D
float Noise = FMath::PerlinNoise1D(Time * Frequency);
// Возвращает -1..1, плавно меняется

// Для покачивания камеры
float ShakeX = FMath::PerlinNoise1D(Time * 5.0f) * Amplitude;
float ShakeY = FMath::PerlinNoise1D(Time * 5.0f + 100.0f) * Amplitude; // Смещение чтобы разные оси не совпадали

// 2D/3D — через FMath::PerlinNoise2D / 3D
float HeightMap = FMath::PerlinNoise2D(FVector2D(X * Scale, Y * Scale));
```

### Weighted Random — взвешенный рандом

```cpp
// Массив весов
TArray<float> Weights = {50.0f, 30.0f, 15.0f, 5.0f};
// Шансы:    50%     30%     15%     5%

int32 WeightedRandom(const TArray<float>& Weights)
{
    float TotalWeight = 0.0f;
    for (float W : Weights) TotalWeight += W;

    float Roll = FMath::FRandRange(0.0f, TotalWeight);
    float Cumulative = 0.0f;

    for (int32 i = 0; i < Weights.Num(); i++)
    {
        Cumulative += Weights[i];
        if (Roll <= Cumulative) return i;
    }
    return Weights.Num() - 1;
}
```

---

## Геймплейная математика

### FOV и конус видимости

```
      \     /
       \   /
        \ /  ← HalfAngle
    Враг ●
        / \
       /   \
      / FOV \
     ●─────── Forward
     NPC
```

```cpp
bool IsInFOV(AActor* Observer, AActor* Target, float HalfAngleDeg)
{
    FVector Forward = Observer->GetActorForwardVector();
    FVector ToTarget = (Target->GetActorLocation() - Observer->GetActorLocation()).GetSafeNormal();

    float Dot = FVector::DotProduct(Forward, ToTarget);
    float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(HalfAngleDeg));

    return Dot >= CosHalfAngle;
}

// Использование: NPC видит игрока в 60° конусе (120° всего)
if (IsInFOV(ZombieActor, PlayerActor, 60.0f))
{
    // Видит!
}
```

### Впереди или позади?

```cpp
FVector Forward = GetActorForwardVector();
FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

float Dot = FVector::DotProduct(Forward, ToTarget);
// Dot > 0 = впереди
// Dot < 0 = позади
// Dot ≈ 0 = сбоку (перпендикулярно)
```

### Слева или справа?

```cpp
FVector Forward = GetActorForwardVector();
FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

FVector Cross = FVector::CrossProduct(Forward, ToTarget);
// Cross.Z > 0 = справа
// Cross.Z < 0 = слева
```

### Расчёт урона по расстоянию (Damage Falloff)

```
Ближе = больше урона, дальше = меньше

Линейный:
  Damage = MaxDamage * (1 - Distance / MaxRange)

Квадратичный (более реалистичный):
  Damage = MaxDamage * (1 - (Distance / MaxRange)²)

С минимумом:
  Damage = FMath::Max(MinDamage, Calculated)
```

```cpp
float CalculateDamage(float BaseDamage, float Distance, float MaxRange, float MinDamagePercent = 0.1f)
{
    float Alpha = FMath::Clamp(Distance / MaxRange, 0.0f, 1.0f);
    float Falloff = 1.0f - Alpha * Alpha; // Квадратичный
    return BaseDamage * FMath::Max(MinDamagePercent, Falloff);
}

// BaseDamage = 100, Distance = 500, MaxRange = 1000
// Alpha = 0.5, Falloff = 1 - 0.25 = 0.75
// Damage = 100 * 0.75 = 75
```

### Разброс оружия (Weapon Spread)

```cpp
FVector ApplySpread(FVector Direction, float SpreadAngleDeg)
{
    // VRandCone генерирует случайное направление в конусе
    return FMath::VRandCone(Direction, FMath::DegreesToRadians(SpreadAngleDeg));
}

// Пример
FVector ShotDir = GetActorForwardVector();
float Spread = bIsAiming ? 1.0f : 5.0f; // ADS точнее

FVector FinalDir = ApplySpread(ShotDir, Spread);
FVector TraceEnd = MuzzleLocation + FinalDir * MaxRange;
```

#### Bloom (увеличение разброса при стрельбе)

```cpp
// Каждый выстрел увеличивает разброс
CurrentSpread += SpreadPerShot;    // Например 0.5°
CurrentSpread = FMath::Min(CurrentSpread, MaxSpread); // Лимит

// Со временем разброс возвращается
void Tick(float DeltaTime)
{
    CurrentSpread = FMath::FInterpTo(CurrentSpread, BaseSpread, DeltaTime, SpreadRecoverySpeed);
}
```

### Наведение на цель (Homing)

```cpp
// Простое наведение — повернуться к цели
void AHomingMissile::Tick(float DeltaTime)
{
    FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();

    // Плавный поворот
    FVector NewDirection = FMath::VInterpNormalRotationTo(
        GetActorForwardVector(), ToTarget, DeltaTime, TurnRate
    );

    SetActorRotation(NewDirection.Rotation());
    SetActorLocation(GetActorLocation() + NewDirection * Speed * DeltaTime);
}
```

### Предсказание позиции (Lead Target)

Стрелять не в текущую позицию цели, а **куда она будет**.

```
Время полёта пули:  t = Distance / BulletSpeed
Позиция цели через t:  PredictedPos = TargetPos + TargetVelocity * t
```

```cpp
FVector PredictTargetPosition(FVector ShooterPos, FVector TargetPos, FVector TargetVel, float BulletSpeed)
{
    float Distance = FVector::Dist(ShooterPos, TargetPos);
    float FlightTime = Distance / BulletSpeed;
    return TargetPos + TargetVel * FlightTime;
}

// Использование
FVector TargetVelocity = Target->GetVelocity();
FVector AimAt = PredictTargetPosition(MuzzlePos, Target->GetActorLocation(), TargetVelocity, BulletSpeed);
FVector ShotDir = (AimAt - MuzzlePos).GetSafeNormal();
```

### Dash / Рывок

```cpp
void AFP_Character::Dash()
{
    FVector DashDirection = GetActorForwardVector(); // Или по направлению инпута
    float DashDistance = 1000.0f;

    // Вариант 1: Мгновенный (телепортация)
    LaunchCharacter(DashDirection * DashDistance, true, true);

    // Вариант 2: Через CharacterMovement
    GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
    LaunchCharacter(DashDirection * DashDistance, true, true);

    // Вернуть трение через таймер
    FTimerHandle Handle;
    GetWorldTimerManager().SetTimer(Handle, [this]()
    {
        GetCharacterMovement()->BrakingFrictionFactor = 2.0f;
    }, 0.3f, false);
}
```

### Knock Back — отбрасывание

```cpp
void ApplyKnockBack(ACharacter* Target, FVector SourceLocation, float Force)
{
    FVector Direction = (Target->GetActorLocation() - SourceLocation).GetSafeNormal();

    // Добавить вертикальную компоненту (подбросить)
    Direction.Z = 0.3f;
    Direction.Normalize();

    Target->LaunchCharacter(Direction * Force, true, true);
}

// Использование: взрыв гранаты
ApplyKnockBack(HitCharacter, ExplosionLocation, 1500.0f);
```

### Camera Shake / Recoil

```cpp
// Отдача оружия (Recoil)
void AFP_Character::ApplyRecoil()
{
    float RecoilPitch = FMath::RandRange(-RecoilAmount, -RecoilAmount * 0.5f); // Всегда вверх
    float RecoilYaw = FMath::RandRange(-RecoilAmount * 0.3f, RecoilAmount * 0.3f);

    AddControllerPitchInput(RecoilPitch);
    AddControllerYawInput(RecoilYaw);
}

// Camera Shake
void AFP_Character::ApplyCameraShake()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        PC->ClientStartCameraShake(FireCameraShakeClass, 1.0f);
    }
}
```

### Формулы урона и левелинга

#### Линейный урон

```
Damage = BaseDamage + (Level - 1) * DamagePerLevel
```

#### Экспоненциальный (RPG-style)

```
Damage = BaseDamage * GrowthFactor ^ (Level - 1)

Пример: Base=10, Growth=1.2
Lvl 1:  10
Lvl 5:  10 * 1.2^4 = 20.7
Lvl 10: 10 * 1.2^9 = 51.6
```

```cpp
float CalcDamage(float Base, float Growth, int32 Level)
{
    return Base * FMath::Pow(Growth, Level - 1);
}
```

#### Опыт для следующего уровня

```
XP = BaseXP * Level ^ Exponent

Пример: Base=100, Exp=1.5
Lvl 1: 100
Lvl 5: 100 * 5^1.5 = 1118
Lvl 10: 100 * 10^1.5 = 3162
```

```cpp
int32 XPForLevel(int32 Level, int32 BaseXP = 100, float Exponent = 1.5f)
{
    return FMath::RoundToInt(BaseXP * FMath::Pow(static_cast<float>(Level), Exponent));
}
```

#### Diminishing Returns (убывающая отдача)

```
Stat = MaxStat * (1 - e^(-Rate * Input))

Чем больше вкладываешь, тем меньше отдача.
```

```cpp
float DiminishingReturn(float Input, float MaxValue, float Rate = 0.01f)
{
    return MaxValue * (1.0f - FMath::Exp(-Rate * Input));
}

// Пример: броня
// DiminishingReturn(100, 80, 0.01) = 47.2% снижения урона
// DiminishingReturn(200, 80, 0.01) = 70.6%
// DiminishingReturn(1000, 80, 0.01) = 79.9% (потолок ~80%)
```

---

## FMath — шпаргалка по функциям UE

### Базовые

```cpp
FMath::Abs(X)                 // Модуль |X|
FMath::Max(A, B)              // Максимум
FMath::Min(A, B)              // Минимум
FMath::Clamp(X, Min, Max)     // Ограничить диапазоном
FMath::Sign(X)                // -1, 0, или 1
FMath::Square(X)              // X²
FMath::Sqrt(X)                // √X
FMath::Pow(Base, Exp)         // Base^Exp
FMath::Loge(X)                // Натуральный логарифм
FMath::Log2(X)                // Логарифм по основанию 2
FMath::Exp(X)                 // e^X
FMath::CeilToInt(X)           // Округление вверх
FMath::FloorToInt(X)          // Округление вниз
FMath::RoundToInt(X)          // Округление к ближайшему
FMath::Fmod(X, Y)             // Остаток от деления (float)
FMath::IsNearlyZero(X, Tol)   // |X| < Tolerance?
FMath::IsNearlyEqual(A, B, Tol) // |A-B| < Tolerance?
```

### Тригонометрия

```cpp
FMath::Sin(Rad)               // Синус
FMath::Cos(Rad)               // Косинус
FMath::Tan(Rad)               // Тангенс
FMath::Asin(X)                // Арксинус → радианы
FMath::Acos(X)                // Арккосинус → радианы
FMath::Atan(X)                // Арктангенс → радианы
FMath::Atan2(Y, X)            // Atan2 → радианы
FMath::SinCos(&S, &C, Rad)    // Sin и Cos за один вызов
FMath::DegreesToRadians(Deg)   // Градусы → радианы
FMath::RadiansToDegrees(Rad)   // Радианы → градусы
```

### Интерполяция

```cpp
FMath::Lerp(A, B, Alpha)              // Линейная
FMath::FInterpTo(Cur, Tar, DT, Spd)   // Float, ease-out
FMath::VInterpTo(Cur, Tar, DT, Spd)   // FVector, ease-out
FMath::RInterpTo(Cur, Tar, DT, Spd)   // FRotator, ease-out
FMath::FInterpConstantTo(...)          // Float, постоянная скорость
FMath::VInterpConstantTo(...)          // FVector, постоянная скорость
FMath::RInterpConstantTo(...)          // FRotator, постоянная скорость
FMath::InterpEaseIn(A, B, Al, Exp)     // Ease In
FMath::InterpEaseOut(A, B, Al, Exp)    // Ease Out
FMath::InterpEaseInOut(A, B, Al, Exp)  // Ease In-Out
FMath::SmoothStep(A, B, Alpha)         // Smooth Hermite
FMath::InterpSpringTo(...)             // Пружина
FMath::CInterpTo(Cur, Tar, DT, Spd)   // FColor
```

### Рандом

```cpp
FMath::Rand()                          // int32 0..RAND_MAX
FMath::FRand()                         // float 0..1
FMath::RandRange(Min, Max)             // int32 [Min, Max]
FMath::FRandRange(Min, Max)            // float [Min, Max]
FMath::RandBool()                      // true/false 50/50
FMath::VRand()                         // Случайный единичный FVector
FMath::VRandCone(Dir, HalfAngleRad)    // Случайный в конусе
FMath::RandPointInBox(Box)             // Случайная точка в боксе
```

### Вектора

```cpp
FVector::DotProduct(A, B)       // A | B — скалярное произведение
FVector::CrossProduct(A, B)     // A ^ B — векторное произведение
FVector::Dist(A, B)             // Расстояние
FVector::Dist2D(A, B)           // Расстояние 2D (X,Y)
FVector::DistSquared(A, B)      // Расстояние² (быстрее)
V.Size()                        // Длина
V.SizeSquared()                 // Длина² (быстрее)
V.Size2D()                      // Длина 2D
V.GetSafeNormal()               // Нормализация (безопасная)
V.Normalize()                   // Нормализация in-place
V.IsNearlyZero()                // Почти нулевой?
V.IsZero()                      // Точно нулевой?
V.Rotation()                    // FVector → FRotator
V.ToDirectionAndLength(Dir, Len) // Разделить на направление + длину
V.ProjectOnTo(Other)            // Проекция на вектор
V.ProjectOnToNormal(Normal)     // Проекция на нормаль
V.MirrorByVector(Normal)        // Отражение
V.RotateAngleAxis(AngleDeg, Axis) // Повернуть вокруг оси
FMath::GetReflectionVector(V, N)   // Отражение
FMath::VInterpNormalRotationTo(Cur, Tar, DT, Speed) // Плавный поворот направления
```

### Mapping (перевод диапазонов)

```cpp
// Перевести значение из одного диапазона в другой
float Result = FMath::GetMappedRangeValueClamped(
    FVector2D(0.0f, 100.0f),     // Входной диапазон
    FVector2D(0.0f, 1.0f),       // Выходной диапазон
    75.0f                         // Значение
);
// 75 в [0,100] → 0.75 в [0,1]

// Без клампа
float Result = FMath::GetMappedRangeValueUnclamped(...);
```

---

## Визуальная шпаргалка

```
┌──────────────────────────────────────────────────────────────────────┐
│                         ВЕКТОРЫ                                      │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  A + B        = Перемещение, сложение скоростей                     │
│  B - A        = Направление из A в B                                │
│  V * scalar   = Масштабирование (длиннее/короче)                    │
│  Normalize(V) = Чистое направление (длина 1)                       │
│  |V|          = Длина (Size)                                        │
│  Dot(A,B)     = Насколько сонаправлены (-1..1)                      │
│  Cross(A,B)   = Перпендикуляр к обоим                               │
│                                                                      │
├──────────────────────────────────────────────────────────────────────┤
│                    DOT PRODUCT                                       │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   Dot =  1.0  →  Одно направление (0°)                             │
│   Dot =  0.0  →  Перпендикулярны (90°)                              │
│   Dot = -1.0  →  Противоположные (180°)                             │
│   Dot >  0.0  →  Впереди                                            │
│   Dot <  0.0  →  Позади                                             │
│                                                                      │
├──────────────────────────────────────────────────────────────────────┤
│                    CROSS PRODUCT (Z компонент)                       │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   Cross.Z > 0  →  Цель СПРАВА                                      │
│   Cross.Z < 0  →  Цель СЛЕВА                                       │
│                                                                      │
├──────────────────────────────────────────────────────────────────────┤
│                    НАПРАВЛЕНИЕ ВЕКТОРА                                │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   End = Start + Direction * Distance                                 │
│         │         │            │                                     │
│         │         │            └─ Как далеко (float)                 │
│         │         └─ Куда (единичный вектор, длина 1)               │
│         └─ Откуда (FVector)                                         │
│                                                                      │
├──────────────────────────────────────────────────────────────────────┤
│                    ИНТЕРПОЛЯЦИЯ                                      │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   Lerp(A, B, 0.0) = A                                               │
│   Lerp(A, B, 0.5) = середина                                       │
│   Lerp(A, B, 1.0) = B                                               │
│                                                                      │
│   FInterpTo      → быстро в начале, медленно в конце (ease-out)     │
│   FInterpConstant → постоянная скорость                              │
│   EaseIn         → медленно → быстро                                │
│   EaseOut        → быстро → медленно                                │
│   EaseInOut      → медленно → быстро → медленно                    │
│                                                                      │
├──────────────────────────────────────────────────────────────────────┤
│                    ФИЗИКА                                            │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   Position += Velocity * DeltaTime                                   │
│   Velocity += Acceleration * DeltaTime                               │
│   Gravity:  Velocity.Z += -980 * DeltaTime                          │
│                                                                      │
│   Force   → постоянное ускорение (каждый кадр)                      │
│   Impulse → мгновенный толчок (один раз)                            │
│                                                                      │
├──────────────────────────────────────────────────────────────────────┤
│                    ОПТИМИЗАЦИЯ                                       │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   ❌ Size() для сравнения         → sqrt дорогой                    │
│   ✅ SizeSquared() для сравнения  → без sqrt                        │
│   ❌ Dist(A,B) для сравнения      → sqrt дорогой                    │
│   ✅ DistSquared(A,B)             → без sqrt                        │
│   ✅ SinCos(&S, &C, Rad)         → оба за один вызов               │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
```

