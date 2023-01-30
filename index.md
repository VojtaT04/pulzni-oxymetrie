# Příručka pro stavbu vlastního oxymetru

Cílem této příručky je předat důležité informace komukoliv, kdo by se rozhodl si náš
oxymetr postavit sám. Pro úspěšnou stavbu se předpokládá, že si člověk dokáže zajistit
vytištění potřebných 3D modelů a má dostatečné znalosti a zkušenosti, aby mohl provést
stavbu bezpečně.

## Seznam potřebných součástek:
- Raspberry Pi Pico
- Vytištěné 3D modely (viz jednotlivé `.stl` soubory)
- Oxymetr - například RCWL-0530, nebo jiný velikostně kompatibilní
- 0.91"128 x 32 OLED displej se sběrnicí I2C
- GeB LiPol Baterie 603048 900 mAh 3.7 V JST-PH
- Nabíječka na Li-ion články - například TP4056
- JST-PH 2 mm 2 piny konektor
- PCB prototypová deska (ideálně oboustranná) o velikosti alespoň 17 x 9 děr
- 2x JST-XH 2.54mm 7 pinů konektor na DPS pravý úhel
- JST-XH 2.54mm 7 pinů vodič se samičím konektorem
- 3x mikrospínač tlačítko (ideálně 6 x 6 x 10 mm, stačí ale i 6 x 6 x 6 mm)
- izolované dráty nebo kabely pro propojování různých součástek
- kus molitanu

Dalšími potřebnými nástroji budou potřeby na pájení, tavná pistole a
ideálně kaptonová páska.

Samozřejmě je možné různé z těchto součástek nahradit za jiné, ale v tu chvíli je důležité si dát pozor a upravit i 3D model pro případnou změnu velikosti.

V případě změny zdroje energie je důležité také zajistit potřebnou ochranu. Dále je třeba si zkontrolovat velikost PCB okolo displeje, naše má rozměry 38,2 x 12,15 mm a na to je dělaný přiložený 3D model, ale existují i displeje s větším PCB, pro jejichž použití by bylo nutné upravit 3D model.


## Stavba
Pokud máte všechny součástky připravené, je čas pustit se do stavby. Celou stavbu budeme dělat podle schématu obvodu ![schéma obvodu](!link!).

### Hardware
1. Začneme horním PCB majícím na sobě displej a tlačítka. PCB prototypovou desku nařežeme tak, aby nám zbyla 46 x 25,6 mm velká část s mřížkou o rozměru 17 x 9 děr.
2. Začneme na ni pájet komponenty.
- Položení komponentů je jasné z 3D tištěného vrchního dílu, který má na konektory, tlačítka i displej jasně určené díry.
- Na spodní straně PCB bychom se měli vyvarovat vyčnívajícím věcem, které by mohly zabránit správnému spojení horního a spodního dílu této části oxymetru. Proto je vhodné zkrátit jakékoli piny které by moc vyčnívaly.
- Horní stranu zato můžeme použít pro kabílky přeskakující jiné části obvodu a případně pullupové rezistory pro dvě z tlačítek, které je sem možno jednoduše schovat.
4. Po připájení všech součástek již stačí PCB vložit mezi dvě půlky horní části oxymetru a dané půlky slepit.
5. Zbytek oxymetru můžeme pájet samostatně mimo samotné 3D tištěné části, ale musíme si dát pozor, protože náš 7 pinový vodič musí být protažen příslušnou dírou v těle oxymetru dříve než bude připájen, a to tak, aby samičí konektor nejen vyčníval ven, ale dal se i ohnout a připojit nahoru do horní části oxymetru.
6. Pokud máme dopájeno, můžeme začít vkládat elektroniku do samotného těla oxymetru.
- První na řadě je oxymetr samotný, který se prostrčí z vnitřku těla oxymetru ven na své místo do měřící komory. Tam je ho nejlepší správně narovnat a ze spodu přilepit tavnou pistolí.
- Dále dovnitř vsuneme samotné Pi Pico tak, aby jeho USB konektor zapadl do jemu příslušné díry, a Pico bylo zabezpečené i zezadu stěnou, jež je tam pro tento účel připravena. Poté již jen stačí umístit zbytek elektroniky. V tuto chvíli je třeba dávat si pozor na potenciální zkraty, kterým může zabránit například kaptonová páska.
7. Teď již stačí jen vše zkontrolovat, připojit akumulátor a zasunout spodní dvířka.

### Software
Z hlediska softwaru je třeba stáhnout Arduino IDE, v Tools > Board: > Boards manager... nainstalovat Arduino Mbed OS RP2040 Boards (verzi 2.7.2 nebo novější). Poté v Tools > Manage libraries... stáhnout MAX30100lib (verzi 1.2.1 nebo novější) a U8g2 (verzi 2.21.2 nebo novější). Poté již stačí připojit oxymetr k počítači, otevřít v Arduino IDE zdrojový kód, v Tools > Board: > Arduino Mbed OS RP2040 Boards > Raspberry Pi Pico a v Tools > Port: vybrat ten, který má u sebe Raspberry Pi Pico (ten je také potřeba znát pro případ využití programu pro stažení dat z oxymetru) a kliknout na tlačítko Upload (šipka doprava).
