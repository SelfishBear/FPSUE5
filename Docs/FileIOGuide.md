# 📂 Полный гайд по работе с файлами в Unreal Engine (C++)

> Чтение, запись, поиск файлов, загрузка ассетов из папок проекта.

---

## Содержание

1. [Основные классы и модули](#1-основные-классы-и-модули)
2. [Важные пути в Unreal Engine](#2-важные-пути-в-unreal-engine)
3. [Чтение текстовых файлов](#3-чтение-текстовых-файлов)
4. [Запись текстовых файлов](#4-запись-текстовых-файлов)
5. [Чтение/запись бинарных файлов](#5-чтениезапись-бинарных-файлов)
6. [Работа с JSON](#6-работа-с-json)
7. [Работа с CSV](#7-работа-с-csv)
8. [Поиск файлов (сканирование директорий)](#8-поиск-файлов-сканирование-директорий)
9. [Проверка существования файлов и папок](#9-проверка-существования-файлов-и-папок)
10. [Создание / удаление / копирование файлов и папок](#10-создание--удаление--копирование-файлов-и-папок)
11. [Чтение файлов из Content папки проекта](#11-чтение-файлов-из-content-папки-проекта)
12. [Загрузка ассетов в рантайме (SoundWave из .mp3)](#12-загрузка-ассетов-в-рантайме-soundwave-из-mp3)
13. [Практический пример: Подсистема для загрузки MP3](#13-практический-пример-подсистема-для-загрузки-mp3)
14. [RuntimeAudioImporter плагин (рекомендация)](#14-runtimeaudioimporter-плагин-рекомендация)
15. [Сводная таблица функций](#15-сводная-таблица-функций)
16. [Частые ошибки и советы](#16-частые-ошибки-и-советы)

---

## 1. Основные классы и модули

| Класс / Заголовок | Для чего |
|---|---|
| `FFileHelper` | Чтение / запись текстовых и бинарных файлов |
| `IFileManager` | Низкоуровневое управление файлами (поиск, удаление, копирование) |
| `FPaths` | Получение стандартных путей проекта |
| `FPlatformFileManager` | Доступ к платформенному файловому менеджеру |
| `IPlatformFile` | Интерфейс платформенной файловой системы |

```cpp
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
```

---

## 2. Важные пути в Unreal Engine

Unreal предоставляет статические методы для получения ключевых директорий:

```cpp
// Корневая папка проекта (где лежит .uproject)
FString ProjectDir = FPaths::ProjectDir();
// Пример: "C:/Projects/FPSGame/"

// Папка Content
FString ContentDir = FPaths::ProjectContentDir();
// Пример: "C:/Projects/FPSGame/Content/"

// Папка Saved
FString SavedDir = FPaths::ProjectSavedDir();
// Пример: "C:/Projects/FPSGame/Saved/"

// Папка Config
FString ConfigDir = FPaths::ProjectConfigDir();
// Пример: "C:/Projects/FPSGame/Config/"

// Папка Plugins
FString PluginsDir = FPaths::ProjectPluginsDir();

// Папка Logs
FString LogDir = FPaths::ProjectLogDir();

// Папка движка
FString EngineDir = FPaths::EngineDir();

// Папка пользователя (AppData и т.д.)
FString UserDir = FPaths::ProjectUserDir();

// Временная папка ОС
FString TempDir = FPaths::EngineIntermediateDir();
```

### Работа с путями

```cpp
// Объединение путей (безопасно, не дублирует слэши)
FString FullPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Audio"), TEXT("music.mp3"));

// Получить расширение файла
FString Extension = FPaths::GetExtension(TEXT("music.mp3")); // "mp3"

// Получить имя файла без пути
FString FileName = FPaths::GetCleanFilename(TEXT("C:/Folder/music.mp3")); // "music.mp3"

// Получить имя без расширения
FString BaseName = FPaths::GetBaseFilename(TEXT("C:/Folder/music.mp3")); // "music"

// Получить директорию из полного пути
FString Dir = FPaths::GetPath(TEXT("C:/Folder/music.mp3")); // "C:/Folder"

// Нормализация пути (замена \ на /, удаление ../ и т.д.)
FPaths::NormalizeFilename(FullPath);

// Превратить относительный путь в абсолютный
FString Absolute = FPaths::ConvertRelativePathToFull(RelativePath);
```

---

## 3. Чтение текстовых файлов

### Весь файл в одну строку

```cpp
FString FilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Data/config.txt"));
FString FileContent;

if (FFileHelper::LoadFileToString(FileContent, *FilePath))
{
    UE_LOG(LogTemp, Log, TEXT("Содержимое файла: %s"), *FileContent);
}
else
{
    UE_LOG(LogTemp, Error, TEXT("Не удалось прочитать файл: %s"), *FilePath);
}
```

### Файл в массив строк (построчно)

```cpp
FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("log.txt"));
TArray<FString> Lines;

if (FFileHelper::LoadFileToStringArray(Lines, *FilePath))
{
    for (const FString& Line : Lines)
    {
        UE_LOG(LogTemp, Log, TEXT("Строка: %s"), *Line);
    }
}
```

### С указанием кодировки

```cpp
// Явно указать UTF-8
FFileHelper::LoadFileToString(FileContent, *FilePath, 
    FFileHelper::EHashOptions::None, 
    FILEREAD_None, 
    &FFileHelper::EEncodingOptions::ForceUTF8);
```

---

## 4. Запись текстовых файлов

### Записать строку в файл (перезапись)

```cpp
FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("output.txt"));
FString Content = TEXT("Hello from Unreal Engine!");

if (FFileHelper::SaveStringToFile(Content, *FilePath))
{
    UE_LOG(LogTemp, Log, TEXT("Файл успешно записан"));
}
```

### Записать строку в файл (UTF-8 без BOM)

```cpp
FFileHelper::SaveStringToFile(
    Content,
    *FilePath,
    FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM
);
```

### Записать массив строк

```cpp
TArray<FString> Lines;
Lines.Add(TEXT("Первая строка"));
Lines.Add(TEXT("Вторая строка"));
Lines.Add(TEXT("Третья строка"));

FFileHelper::SaveStringArrayToFile(Lines, *FilePath);
```

### Дописать в конец файла (Append)

```cpp
FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("log.txt"));
FString NewLine = TEXT("Новая запись в лог\n");

FFileHelper::SaveStringToFile(
    NewLine,
    *FilePath,
    FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,
    &IFileManager::Get(),
    FILEWRITE_Append  // <-- Ключевой флаг: дописать, а не перезаписать
);
```

---

## 5. Чтение/запись бинарных файлов

### Чтение в массив байтов

```cpp
FString FilePath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data/image.png"));
TArray<uint8> FileData;

if (FFileHelper::LoadFileToArray(FileData, *FilePath))
{
    UE_LOG(LogTemp, Log, TEXT("Прочитано %d байт"), FileData.Num());
}
```

### Запись массива байтов

```cpp
TArray<uint8> Data;
Data.Add(0xFF);
Data.Add(0x00);
Data.Add(0xAB);

FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("data.bin"));
FFileHelper::SaveArrayToFile(Data, *FilePath);
```

---

## 6. Работа с JSON

### Чтение JSON из файла

```cpp
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

FString FilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Data/config.json"));
FString JsonString;

if (FFileHelper::LoadFileToString(JsonString, *FilePath))
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        FString Name = JsonObject->GetStringField(TEXT("name"));
        int32 Value  = JsonObject->GetIntegerField(TEXT("value"));
        bool bFlag   = JsonObject->GetBoolField(TEXT("enabled"));

        // Вложенный объект
        TSharedPtr<FJsonObject> Inner = JsonObject->GetObjectField(TEXT("settings"));

        // Массив
        TArray<TSharedPtr<FJsonValue>> Items = JsonObject->GetArrayField(TEXT("items"));
        for (auto& Item : Items)
        {
            FString ItemStr = Item->AsString();
        }
    }
}
```

### Запись JSON в файл

```cpp
#include "Serialization/JsonWriter.h"

TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
JsonObject->SetStringField(TEXT("name"), TEXT("Rifle"));
JsonObject->SetNumberField(TEXT("damage"), 50);
JsonObject->SetBoolField(TEXT("automatic"), true);

// Массив
TArray<TSharedPtr<FJsonValue>> Ammo;
Ammo.Add(MakeShareable(new FJsonValueString(TEXT("9mm"))));
Ammo.Add(MakeShareable(new FJsonValueString(TEXT("5.56"))));
JsonObject->SetArrayField(TEXT("ammo_types"), Ammo);

FString OutputString;
TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("weapons.json"));
FFileHelper::SaveStringToFile(OutputString, *FilePath);
```

---

## 7. Работа с CSV

```cpp
FString FilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Data/stats.csv"));
TArray<FString> Lines;

if (FFileHelper::LoadFileToStringArray(Lines, *FilePath))
{
    for (int32 i = 1; i < Lines.Num(); i++) // Пропускаем заголовок
    {
        TArray<FString> Columns;
        Lines[i].ParseIntoArray(Columns, TEXT(","), true);

        if (Columns.Num() >= 3)
        {
            FString Name   = Columns[0];
            int32 HP       = FCString::Atoi(*Columns[1]);
            float Speed    = FCString::Atof(*Columns[2]);
        }
    }
}
```

---

## 8. Поиск файлов (сканирование директорий)

### Найти все файлы с определённым расширением

```cpp
FString SearchDir = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Audio"));
TArray<FString> FoundFiles;

// Найти все .mp3 файлы (рекурсивно по подпапкам)
IFileManager::Get().FindFilesRecursive(
    FoundFiles,          // Результат
    *SearchDir,          // Где искать
    TEXT("*.mp3"),       // Паттерн
    true,                // Искать файлы
    false,               // Не искать директории
    false                // Не очищать массив перед добавлением (false = очищать)
);

for (const FString& File : FoundFiles)
{
    UE_LOG(LogTemp, Log, TEXT("Найден файл: %s"), *File);
}
```

### Найти файлы только в одной папке (без рекурсии)

```cpp
TArray<FString> FoundFiles;

// Вернёт только имена файлов (без полного пути!)
IFileManager::Get().FindFiles(FoundFiles, *SearchDir, TEXT("*.mp3"));

for (const FString& FileName : FoundFiles)
{
    FString FullPath = FPaths::Combine(SearchDir, FileName);
    UE_LOG(LogTemp, Log, TEXT("Файл: %s"), *FullPath);
}
```

### Найти все файлы любого типа

```cpp
TArray<FString> AllFiles;
IFileManager::Get().FindFilesRecursive(
    AllFiles,
    *FPaths::ProjectContentDir(),
    TEXT("*.*"),   // Любые файлы
    true,          // Файлы — да
    false          // Директории — нет
);
```

### Найти только директории

```cpp
TArray<FString> Directories;
IFileManager::Get().FindFilesRecursive(
    Directories,
    *FPaths::ProjectContentDir(),
    TEXT("*"),
    false,         // Файлы — нет
    true           // Директории — да
);
```

---

## 9. Проверка существования файлов и папок

```cpp
FString FilePath = TEXT("C:/Data/test.txt");
FString DirPath  = TEXT("C:/Data/Sounds");

// Файл существует?
bool bFileExists = FPaths::FileExists(FilePath);

// Директория существует?
bool bDirExists = FPaths::DirectoryExists(DirPath);

// Через IPlatformFile (более низкоуровневый вариант)
IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
bool bExists = PlatformFile.FileExists(*FilePath);
bool bIsDir  = PlatformFile.DirectoryExists(*DirPath);

// Размер файла (-1 если не существует)
int64 FileSize = PlatformFile.FileSize(*FilePath);

// Время последнего изменения
FDateTime ModTime = PlatformFile.GetTimeStamp(*FilePath);

// Только для чтения?
bool bReadOnly = PlatformFile.IsReadOnly(*FilePath);
```

---

## 10. Создание / удаление / копирование файлов и папок

```cpp
IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

// Создать директорию (включая все промежуточные)
PlatformFile.CreateDirectoryTree(TEXT("C:/Data/NewFolder/SubFolder"));

// Удалить файл
IFileManager::Get().Delete(TEXT("C:/Data/old.txt"));

// Удалить директорию (true = рекурсивно)
IFileManager::Get().DeleteDirectory(TEXT("C:/Data/OldFolder"), false, true);

// Копировать файл
IFileManager::Get().Copy(
    TEXT("C:/Data/destination.txt"),  // Куда
    TEXT("C:/Data/source.txt")        // Откуда
);

// Переместить файл (переименовать)
IFileManager::Get().Move(
    TEXT("C:/Data/new_name.txt"),     // Новый путь
    TEXT("C:/Data/old_name.txt")      // Старый путь
);
```

---

## 11. Чтение файлов из Content папки проекта

Файлы в `Content/` — это **uasset** ассеты, но можно хранить и raw-файлы.

### Вариант А: Хранить raw-файлы рядом с Content

Структура:
```
FPSGame/
├── Content/
├── RawData/          <-- Наша папка с raw файлами
│   └── Audio/
│       ├── track01.mp3
│       ├── track02.mp3
│       └── track03.mp3
```

```cpp
FString AudioDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("RawData/Audio"));
TArray<FString> MP3Files;

IFileManager::Get().FindFilesRecursive(
    MP3Files, *AudioDir, TEXT("*.mp3"), true, false
);
```

### Вариант Б: Хранить raw-файлы в Content

> ⚠️ UE может импортировать .mp3 как SoundWave при помещении в Content.
> Для raw-файлов лучше использовать папку вне Content.

### Вариант В: Читать из Saved

```cpp
FString AudioDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("CustomAudio"));
```

### Вариант Г: Читать с абсолютного пути

```cpp
FString AudioDir = TEXT("D:/MyMusic");
TArray<FString> MP3Files;
IFileManager::Get().FindFilesRecursive(MP3Files, *AudioDir, TEXT("*.mp3"), true, false);
```

---

## 12. Загрузка ассетов в рантайме (SoundWave из .mp3)

### Стандартный способ — ImportedSoundWave (UE 5.4+)

С UE 5.4 появился `URuntimeAudioImporterLibrary` или можно использовать `USoundWaveProcedural`.

### Ручной способ через бинарное чтение

```cpp
#include "Sound/SoundWave.h"

USoundWave* LoadSoundWaveFromFile(const FString& FilePath)
{
    TArray<uint8> RawData;
    if (!FFileHelper::LoadFileToArray(RawData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Не удалось загрузить файл: %s"), *FilePath);
        return nullptr;
    }

    // Для полноценной работы нужен плагин RuntimeAudioImporter
    // или ручной парсинг MP3 формата.
    // USoundWave напрямую не умеет загружать MP3 из сырых данных без декодера.

    return nullptr; // Заглушка — используй RuntimeAudioImporter
}
```

> ⚠️ **Важно:** Unreal Engine **не умеет** нативно декодировать MP3 из raw-байтов в рантайме.
> Для этого нужен плагин **RuntimeAudioImporter** (бесплатный на GitHub).

---

## 13. Практический пример: Подсистема для загрузки MP3

Пример Game Instance Subsystem которая при старте сканирует папку проекта и собирает пути к MP3 файлам.

### MusicSubsystem.h

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MusicSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FMusicTrackInfo
{
    GENERATED_BODY()

    // Полный путь к файлу на диске
    UPROPERTY(BlueprintReadOnly, Category = "Music")
    FString FilePath;

    // Имя файла (без пути и расширения)
    UPROPERTY(BlueprintReadOnly, Category = "Music")
    FString TrackName;
};

UCLASS()
class FPSGAME_API UMusicSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Сканировать папку и найти все MP3
    UFUNCTION(BlueprintCallable, Category = "Music")
    void ScanMusicFolder();

    // Сканировать произвольную папку
    UFUNCTION(BlueprintCallable, Category = "Music")
    void ScanCustomFolder(const FString& FolderPath);

    // Получить все найденные треки
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
    const TArray<FMusicTrackInfo>& GetAllTracks() const { return Tracks; }

    // Получить трек по индексу
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
    bool GetTrackByIndex(int32 Index, FMusicTrackInfo& OutTrack) const;

    // Количество треков
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Music")
    int32 GetTrackCount() const { return Tracks.Num(); }

    // Прочитать raw-данные трека в массив байтов
    UFUNCTION(BlueprintCallable, Category = "Music")
    bool LoadTrackRawData(int32 Index, TArray<uint8>& OutData) const;

protected:
    // Папка по умолчанию (относительно корня проекта)
    UPROPERTY(EditDefaultsOnly, Category = "Music")
    FString DefaultMusicFolder = TEXT("RawData/Audio");

private:
    UPROPERTY()
    TArray<FMusicTrackInfo> Tracks;

    void ScanDirectory(const FString& AbsolutePath);
};
```

### MusicSubsystem.cpp

```cpp
#include "MusicSubsystem.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

void UMusicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Автоматически сканируем при старте
    ScanMusicFolder();
}

void UMusicSubsystem::ScanMusicFolder()
{
    FString AbsolutePath = FPaths::Combine(FPaths::ProjectDir(), DefaultMusicFolder);
    AbsolutePath = FPaths::ConvertRelativePathToFull(AbsolutePath);

    ScanDirectory(AbsolutePath);
}

void UMusicSubsystem::ScanCustomFolder(const FString& FolderPath)
{
    FString AbsolutePath = FPaths::ConvertRelativePathToFull(FolderPath);
    ScanDirectory(AbsolutePath);
}

void UMusicSubsystem::ScanDirectory(const FString& AbsolutePath)
{
    Tracks.Empty();

    if (!FPaths::DirectoryExists(AbsolutePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("[MusicSubsystem] Папка не найдена: %s"), *AbsolutePath);
        return;
    }

    TArray<FString> FoundFiles;
    IFileManager::Get().FindFilesRecursive(
        FoundFiles,
        *AbsolutePath,
        TEXT("*.mp3"),
        true,   // Искать файлы
        false   // Не искать директории
    );

    for (const FString& File : FoundFiles)
    {
        FMusicTrackInfo TrackInfo;
        TrackInfo.FilePath  = File;
        TrackInfo.TrackName = FPaths::GetBaseFilename(File);

        Tracks.Add(TrackInfo);

        UE_LOG(LogTemp, Log, TEXT("[MusicSubsystem] Найден трек: %s"), *TrackInfo.TrackName);
    }

    UE_LOG(LogTemp, Log, TEXT("[MusicSubsystem] Всего треков: %d"), Tracks.Num());
}

bool UMusicSubsystem::GetTrackByIndex(int32 Index, FMusicTrackInfo& OutTrack) const
{
    if (!Tracks.IsValidIndex(Index))
    {
        return false;
    }

    OutTrack = Tracks[Index];
    return true;
}

bool UMusicSubsystem::LoadTrackRawData(int32 Index, TArray<uint8>& OutData) const
{
    if (!Tracks.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Error, TEXT("[MusicSubsystem] Неверный индекс: %d"), Index);
        return false;
    }

    const FString& FilePath = Tracks[Index].FilePath;

    if (!FFileHelper::LoadFileToArray(OutData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("[MusicSubsystem] Не удалось загрузить: %s"), *FilePath);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[MusicSubsystem] Загружено %d байт из %s"),
        OutData.Num(), *FilePath);

    return true;
}
```

### Использование в другом классе

```cpp
// Получить подсистему
UMusicSubsystem* MusicSub = GetGameInstance()->GetSubsystem<UMusicSubsystem>();

// Все треки
const TArray<FMusicTrackInfo>& AllTracks = MusicSub->GetAllTracks();

for (const FMusicTrackInfo& Track : AllTracks)
{
    UE_LOG(LogTemp, Log, TEXT("Трек: %s | Путь: %s"), *Track.TrackName, *Track.FilePath);
}

// Загрузить raw-данные конкретного трека
TArray<uint8> RawAudio;
if (MusicSub->LoadTrackRawData(0, RawAudio))
{
    // Передать в RuntimeAudioImporter для декодирования
}
```

### Использование в Blueprint

```
Get Game Instance → Get Subsystem (MusicSubsystem)
    → Scan Music Folder
    → Get All Tracks
    → For Each Loop
        → Print: Track Name
```

---

## 14. RuntimeAudioImporter плагин (рекомендация)

Для реального воспроизведения MP3 в рантайме используй бесплатный плагин:

🔗 **RuntimeAudioImporter** — https://github.com/gtreshchev/RuntimeAudioImporter

### Установка

1. Скачать с GitHub или Fab
2. Поместить в `Plugins/`
3. Добавить в `.Build.cs`: `"RuntimeAudioImporter"`

### Пример использования с подсистемой

```cpp
#include "RuntimeAudioImporterLibrary.h"

void UMusicSubsystem::ImportAndPlay(int32 Index, UAudioComponent* AudioComponent)
{
    TArray<uint8> RawData;
    if (!LoadTrackRawData(Index, RawData))
    {
        return;
    }

    URuntimeAudioImporterLibrary* Importer = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();

    Importer->OnResultNative.AddLambda([AudioComponent](
        URuntimeAudioImporterLibrary* ImporterRef,
        UImportedSoundWave* SoundWave,
        ERuntimeImportStatus Status)
    {
        if (Status == ERuntimeImportStatus::SuccessfulImport && AudioComponent)
        {
            AudioComponent->SetSound(SoundWave);
            AudioComponent->Play();
        }
    });

    Importer->ImportAudioFromBuffer(RawData, ERuntimeAudioFormat::Mp3);
}
```

---

## 15. Сводная таблица функций

### FFileHelper (чтение / запись)

| Метод | Описание |
|---|---|
| `LoadFileToString(OutStr, Path)` | Прочитать текстовый файл в FString |
| `LoadFileToStringArray(OutArr, Path)` | Прочитать файл построчно в TArray<FString> |
| `LoadFileToArray(OutArr, Path)` | Прочитать бинарный файл в TArray<uint8> |
| `SaveStringToFile(Str, Path)` | Записать FString в файл |
| `SaveStringArrayToFile(Arr, Path)` | Записать TArray<FString> построчно |
| `SaveArrayToFile(Arr, Path)` | Записать TArray<uint8> в бинарный файл |

### FPaths (пути)

| Метод | Описание |
|---|---|
| `ProjectDir()` | Корень проекта |
| `ProjectContentDir()` | Папка Content |
| `ProjectSavedDir()` | Папка Saved |
| `ProjectConfigDir()` | Папка Config |
| `ProjectPluginsDir()` | Папка Plugins |
| `ProjectLogDir()` | Папка Logs |
| `Combine(A, B)` | Безопасное объединение путей |
| `FileExists(Path)` | Существует ли файл |
| `DirectoryExists(Path)` | Существует ли папка |
| `GetExtension(Path)` | Расширение файла |
| `GetCleanFilename(Path)` | Имя файла с расширением |
| `GetBaseFilename(Path)` | Имя файла без расширения |
| `GetPath(Path)` | Только директория из пути |
| `ConvertRelativePathToFull(Path)` | Относительный → абсолютный |
| `NormalizeFilename(Path)` | Нормализация слэшей |

### IFileManager (файловые операции)

| Метод | Описание |
|---|---|
| `FindFiles(Out, Dir, Ext)` | Найти файлы в папке (неглубоко) |
| `FindFilesRecursive(...)` | Рекурсивный поиск файлов |
| `Delete(Path)` | Удалить файл |
| `DeleteDirectory(Path, R, T)` | Удалить директорию |
| `Copy(Dst, Src)` | Копировать файл |
| `Move(Dst, Src)` | Переместить / переименовать |
| `MakeDirectory(Path, Tree)` | Создать директорию |
| `FileSize(Path)` | Размер файла |
| `GetTimeStamp(Path)` | Время последнего изменения |

---

## 16. Частые ошибки и советы

### ❌ Слэши

Windows использует `\`, Unreal использует `/`. Всегда используй `FPaths::Combine()` и `FPaths::NormalizeFilename()`.

```cpp
// ❌ Плохо
FString Path = TEXT("C:\\Data\\file.txt");

// ✅ Хорошо
FString Path = FPaths::Combine(TEXT("C:/Data"), TEXT("file.txt"));
```

### ❌ Относительные пути

Относительные пути в Unreal зависят от рабочей директории, которая может отличаться в редакторе и в билде.

```cpp
// ❌ Ненадёжно
FString Path = TEXT("../Data/file.txt");

// ✅ Надёжно — всегда от известного корня
FString Path = FPaths::Combine(FPaths::ProjectDir(), TEXT("Data/file.txt"));
```

### ❌ Чтение в Game Thread

Чтение больших файлов на Game Thread заблокирует игру. Для больших файлов используй `AsyncTask`:

```cpp
AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
{
    TArray<uint8> Data;
    FFileHelper::LoadFileToArray(Data, TEXT("C:/Data/big_file.dat"));

    // Вернуться на Game Thread для обработки
    AsyncTask(ENamedThreads::GameThread, [this, Data = MoveTemp(Data)]()
    {
        ProcessData(Data);
    });
});
```

### ❌ Забыть проверить существование

Всегда проверяй результат операций:

```cpp
// ✅ Проверяем результат
if (!FFileHelper::LoadFileToString(Content, *Path))
{
    UE_LOG(LogTemp, Error, TEXT("Файл не найден: %s"), *Path);
    return;
}
```

### ❌ Путь в Packaged Build

В запакованном билде `FPaths::ProjectContentDir()` указывает на `Content/Paks/`. Raw-файлы туда не попадают!

Решение — класть raw-файлы в папку `Saved/` или рядом с `.exe`:

```cpp
// Путь рядом с исполняемым файлом
FString ExeDir = FPaths::LaunchDir();
```

### 💡 Совет: Non-Asset директории в паке

Чтобы включить сырые файлы в packaged build, добавь в `DefaultGame.ini`:

```ini
[/Script/UnrealEd.ProjectPackagingSettings]
+DirectoriesToAlwaysStageAsNonUFS=(Path="RawData/Audio")
```

Это скопирует папку `RawData/Audio` в билд как есть (не в .pak).

