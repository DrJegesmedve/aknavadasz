# AKNAVADÁSZ - README
==============================================================

## A JÁTÉKRÓL

Az Aknavadász egy klasszikus aknakereső játék implementációja, ahol a feladat megtalálni és elkerülni az aknákat egy 10x10-es játéktáblán. A játékos célja, hogy feltárja az összes aknamentes mezőt anélkül, hogy bármelyik aknára lépne.

## FŐBB FUNKCIÓK

- 10x10-es játéktábla
- Többféle nehézségi szint (könnyű: 10 akna, közepes: 25 akna, nehéz: 50 akna)
- Egyedi nehézségi beállítás lehetősége (0-100 akna között)
- Egyszerű billentyűzetes irányítás
- Dinamikus pályagenerálás

## JÁTÉKMENET

1. A játékos egy karakterrel (P) mozog a pályán
2. Amikor egy mezőre lép és feltárja azt:
   - Ha akna van ott, a játék véget ér
   - Ha nincs akna, megjelenik egy szám, amely jelzi a szomszédos aknák számát
   - Ha nincs szomszédos akna (0), a környező mezők automatikusan feltárulnak
3. A játékos megjelölheti (F) az aknának vélt mezőket
4. A játék akkor ér véget, ha a játékos:
   - Aknára lép (vereség)
   - Minden aknamentes mezőt feltár (győzelem)
   - Megszakítja a játékot (ESC)

## IRÁNYÍTÁS

### Menünavigáció:
- Nyílbillentyűk: Navigálás a menükben
- ENTER: Kiválasztás
- ESC: Vissza / Kilépés

### Nehézség beállítása:
- A/D vagy Bal/Jobb nyíl: Nehézségi szint váltása
- W/S vagy Fel/Le nyíl: Bombák számának növelése/csökkentése

### Játék közben:
- Nyílbillentyűk: Játékos mozgatása
- ENTER: Mező feltárása
- BACKSPACE: Mező megjelölése/jelölés törlése
- ESC: Játék megszakítása

## JELÖLÉSEK

- P: Játékos pozíciója
- X: Feltáratlan mező
- F: Megjelölt mező (feltételezett akna)
- Szám (1-8): Szomszédos aknák száma
- *: Akna (csak játék vége után látható)
- Üres mező (' '): Nincs szomszédos akna

## TELEPÍTÉS ÉS FUTTATÁS

1. Fordítsa le a kódot C++ fordítóval (pl. Visual Studio, g++)
2. Futtassa a létrejött .exe fájlt
3. A program Windows operációs rendszeren fut optimálisan

## TECHNIKAI INFORMÁCIÓK

- C++ nyelven íródott
- Windows konzolalkalmazás
- Használt könyvtárak: iostream, windows.h, conio.h, string, vector, functional, ctime

## KÉSZÍTŐK

- Fejlesztő: DrJegesmedve
- Designer: DrJegesmedve
- Tesztelők: 
  - cnek
  - Szasza
  - dr.frustrumassae
  - K.O.

==============================================================

Köszönjük, hogy az Aknavadászt választottad! Jó szórakozást!
