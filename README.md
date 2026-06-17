# Open Mars

> Adam Cofała (343007), Jakub Sałata (343064)

---


## 1. O grze

**Open Mars** to turbo-wczesna alfa gry w stylu Transport Tycoon / Factorio, ale na Marsie. Gracz terraformuje i zasiedla proceduralnie generowaną mapę, budując infrastrukturę energetyczną i wodną, która pozwala kolonii przeżyć.

### Mechaniki

**Generowanie świata**
Mapa powstaje z szumem Perlina z domain warpingiem – można regulować chropowatość, wysokość terenu, siłę warpingu i próg lodowych złóż. Teren jest kwadratowy (64×64 do 192×192), a każdy kafelek może być płaski lub mieć jeden z czterech typów nachylenia.

**Izometryczny rendering**
Kafelki renderowane są w rzucie izometrycznym (painter’s algorithm, back-to-front). Każdy kafelek ma 4 narożniki z indywidualnymi wysokościami, co daje płynne, rzeźbiarskie zbocza. Rury mają specjalną logikę sortowania.

**Budynki**

- **Solar Panel** – produkuje energię; wydajność rośnie z wysokością terenu
- **Ice Melter** – konsumuje energię, produkuje wodę; musi stać na lodzie
- **Water Magazine** – zbiornik wody z wejściem i wyjściem
- **Pipe** – łączy budynki w sieci przesyłowe

**Sieci rur (PipeNetwork)**
Rury wykrywają sąsiadów automatycznie i łączą się w logiczne sieci. Każda sieć ma typ materiału (woda / energia), listę producentów i konsumentów. Każdą turę zasoby są dystrybuowane proporcjonalnie między konsumentów.

**Czas gry**
Świat ma własny kalendarz (dzień / miesiąc / rok, start: 2050). Prędkość symulacji można skalować z GUI. Wszystko można zapisać i wczytać binarnie.

---

## 2. Struktura klas

```
Game
├── World
│   ├── Map
│   │   ├── Tile
│   │   ├── Structure
│   │   │   ├── SolarPanel
│   │   │   ├── IceMelter
│   │   │   ├── WaterMagazine
│   │   │   └── Pipe
│   │   └── PipeNetwork
│   └── Inventory
├── Renderer
│   ├── GameCamera
│   └── TextureManager
├── InputManager
├── Gui
└── StartScreen
```

| Klasa | Co robi |
|---|---|
| `Game` | Główna pętla, state machine (StartScreen / Playing) |
| `World` | Trzyma mapę, inwentarz globalny i czas gry |
| `Map` | Siatka kafelków, lista struktur, sieci rur, logika stawiania/burzenia |
| `Tile` | Pojedynczy kafelek – poziom, nachylenie (4 bity), typ, wskaźnik na strukturę |
| `Structure` | Klasa bazowa – pozycja, porty, dwa inwentarze, raty produkcji/konsumpcji |
| `SolarPanel` | Produkuje energię, więcej na wyższym terenie |
| `IceMelter` | Zamienia energię w wodę, musi stać na lodzie |
| `WaterMagazine` | Zbiornik wody z wejściem i wyjściem |
| `Pipe` | Rura – maska połączeń (4 bity), typ materiału, referencja do sieci |
| `PipeNetwork` | Logiczna sieć rur – dystrybuuje zasoby od producentów do konsumentów |
| `Inventory` | Magazyn zasobów z kontrolą pojemności |
| `Renderer` | Izometryczny rendering terenu i struktur, painter’s algorithm |
| `GameCamera` | Kamera 2D – pan (WSAD/RMB), smooth zoom do pozycji myszy |
| `TextureManager` | Ładuje atlasy tekstur, lookup bitowy -> maska -> prostokąt |
| `InputManager` | Zaznaczanie kafelków (ray-cast na height-mapy), stawianie, burzenie |
| `Gui` | Panel ImGui – narzędzia, budynki, prędkość, zapis/odczyt |
| `StartScreen` | Animowane menu główne, kreator świata, ekran ładowania |

---

## 3. Co planowaliśmy vs. co wyszło

### Wyszło

- Proceduralne generowanie terenu z Perlin noise + domain warping + saddle-fix
- Izometryczny renderer z detekcją kafelków po mapie wysokości (height-map tile-picking)
- System rur z automatycznym wykrywaniem sieci i propagacją materiałów
- Pełny zapis/odczyt binarny (mapa, struktury, sieci, inwentarz, czas)
- Animowany start screen (pikselowe góry, gwiazdy, słońce po paraboli)
- Demolish tool z podglądem usuwania
- Regulacja prędkości symulacji

### Nie wyszło / planujemy dodać

- **Bardziej zaawansowane budynki** – SolarPanel i IceMelter mają offset 3×3, ale logika kolizji jest prosta i nie obsługuje rotacji
- **AI kolonistów** – planowano agentów poruszających się po mapie
- **Więcej surowców** – struktura `MaterialType` jest przygotowana na rozszerzenie, ale gra ma tylko energię i wodę
- **Badania / tech tree** – nie zaczęte
- **Dźwięk** – muzyczka, efekty stawiania
- **Kampania / misje** – gra jest sandbox bez żadnego celu końcowego

---

## 4. Wnioski

### Wnioski ogólne

- Architektura oparta na izolowaniu logiki w oddzielnych klasach (Map, Tile, Structure) sprawdziła się przy zarządzaniu stanem, jednak rozbudowa o nowe typy budynków wymagała ręcznych zmian w wielu miejscach (switch/case), co spowalnia aktualizowanie o nowe struktury.

- Zapis/odczyt świata okazał się wydajny i prosty w implementacji, ale brak wersjonowania schematu danych utrudni długoterminową kompatybilność zapisów.

- Najwięcej czasu pochłonęło zapewnienie spójności między logiczną reprezentacją sieci rur a ich wyświetlaniem – szczególnie w przypadku dynamicznego dodawania i usuwania połączeń.

- Projekty typu sandbox bez zdefiniowanego celu końcowego cierpią na brak wyraźnego zakończenia; warto byłoby już na wczesnym etapie wprowadzić choćby proste zadania, aby nadać rozgrywce kierunek.

### Użycie AI

AI znacząco przyspieszyło pisanie szablonowego kodu: generowanie klas save/load, układy ImGui, implementację podstawowych rzeczy w oop np. getter'ów. Wyzwania pojawiły się przy logice ściśle powiązanej z architekturą projektu – np. algorytm porządkowania rur (pipe reordering) wymagał wielu prób i ręcznych poprawek, ponieważ model nie rozumiał pełnego kontekstu systemu. Wniosek: AI doskonale sprawdza się przy szybkim tworzeniu szkieletu, ale przy złożonych zależnościach między modułami konieczne jest samodzielne myślenie.