# Morse-code-terminal-with-receiver-transmitter-function
Project based on KL05Z

Program realizowany jako terminal nadawczo - odbiorczy
Za pomocą pojedyńczego przycisku użytkownik kodem Morse'a nadaje konkretną literę, która następnie zostaje przedstawiona na pierwszym rzędzie wyświetlacza LCD a jednocześnie wysłana do drugiego terminala. Jednocześnie do układu mogą zostać oderbane dane wysłanego z drugiego terminala

Prezentacja działania: Youtube

Końcowa kompilacja
Program Size: Code=3928 RO-data=420 RW-data=8 ZI-data=360  
".\Objects\Project_Morse.axf" - 0 Error(s), 0 Warning(s).

# W programie zrealizowano następujące funkcje:
1) Brak delay-i blokujących program
2) Programowa obsługa drgań styków z możliwością dodania dodatkowych klawiszy (uniwersalność) - należy ustawić odpowiednio pin, dodać strukturę, dodać w pętli głównej funkcje obsługującą konkretny przycisk - wykorzystano PIT
3) Przesuwanie tekstu na wyświetlaczu - w przypadku zbyt dużej ilości znaków program "przesuwa" znaki w lewo tworząc nowe miejsce dla nowych znaków
4) Wyświetlanie zdekodowanych znaków na pierwszym rzędzie i odebranych z drugiego terminala na drugim rzędzie
5) Niezawodny i nieblokujący sposób dekodowania znaków oparty na czasach z PIT-a
6) Brak sztywnych ram czasowych odbioru - program samodzielnie przystosowuje się do prędkości wprowadzania danych przez użytkownika (ważna funkcja dla początkujących telegrafistów)
7) Program nie blokuje się nawet przy dużej szybkości odbioru danych z innego terminala
8) Bezprzewodowa komunikacja z drugim terminalem nawet do 1km

# Wykorzystane podzespoły:
1) Płytka KL05Z
2) Przycisk do nadawania kodu Morse'a
3) Wyświetlacz LCD1602 z przedstawieniem ruchu odbieranego i nadawanego
4) Dwa moduły komunikacyjne HC-11 - jeden po stronie KL05Z drugi po stronie komputera-terminala (Komunikacja po UART)

# Dynamiczna zmiana prędkości
W celu dobrania prędkości nadawania Morse'em użytkownik może sprzyspieszyć działanie dekodera Morse'a
1) przyspieszenie - krótkie wciśnięcie przycisku (kropka) - skórcenie średniej długości kreska/kropka
2) spowolnienie - długie wciśnięcie przycisku (kreska) - wydłużenie średniej długości kreska/kropka
Użytkownik może w dowolnym momencie (nawet przy nadawaniu) zmienić prędkość nadawania znaków.

# Diagram przejść kodu Morse'a
<p align="center">
  <img src="img/morse_chart.jpg" width="800" title="hover text">
</p>

# Autorzy:
Igor Kamiński EiT3
