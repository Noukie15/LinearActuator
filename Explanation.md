# Uitleg project

Ik werk aan een Arduino Uno-project voor een lineair positioneringssysteem. Een geborstelde DC-motor drijft via een L298N H-brug een M6-schroefdraadstang aan met een aangenomen spoed van 1 mm. Een moer verplaatst het karretje. De positie wordt gemeten met een zelfgemaakte incrementele quadratuur-encoder met twee digitale kanalen, aangesloten op D2 en D3. De encoder heeft volgens de huidige aannames 20 gaten en wordt x4 uitgelezen, wat neerkomt op 80 tellingen per spindelomwenteling en theoretisch 0,0125 mm per telling.

Ik gebruik de Arduino-libraries `Encoder` van Paul Stoffregen en `PID_v1` van Brett Beauregard. De L298N is aangesloten met ENA op D9 (PWM), IN1 op D7 en IN2 op D8. De huidige PID-startwaarden zijn Kp = 80, Ki = 0 en Kd = 3; deze zijn nog niet afgestemd.

De huidige code gebruikt een vaste tekenbuffer voor seriële commando's. De commando's zijn `P1` (10 mm), `P2` (50 mm), `P3` (90 mm), `G 25` (willekeurige positie in mm), `ZERO` (huidige positie op nul zetten) en `STOP`. De bewegingsgrenzen zijn momenteel 0 tot 100 mm. Er zijn geen automatische testfuncties nodig in de huidige eenvoudige versie.

Geef prioriteit aan eenvoudige, efficiënte en betrouwbare Arduino-code. Verander de pinnen, mechanische aannames of commando's niet zonder dit expliciet te vermelden. Controleer technische aannames in plaats van ze zomaar als correct te beschouwen. Maak duidelijk onderscheid tussen encoderresolutie en echte positioneringsnauwkeurigheid. Stel bij codewijzigingen bij voorkeur volledige, compileerbare code voor en vermeld eventuele beperkingen en veiligheidsrisico's.
