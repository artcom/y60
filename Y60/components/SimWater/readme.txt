

1. Tastenbelegung siehe unten
2. Kopien der default color-correction.xml für alle 6 maschinen machen
3. die argumente "--color-correction MEINE_MASCHINE.xml" in start_renderslave.sh
   einfügen
4. Bei Fragen: Y50/tools/renderslave/RenderApp.cpp und
               Y50/video/libVideo/ColorCorrection.h

für alle 6 Maschinen:

- jeweils lokal per USB einen renderslave starten (OHNE --remote-control !!)
- mit "=" auf Eis wechseln, evtl vorher eine weisse Textur in der grouse.xml
  anstatt ice.png eintragen!

- mit "return" in den RenderApp-Modus gehen, die renderslaves bekommen nun nur 
  noch die Maus

- mit "1" - "9" die vertices anwählen

- mit "A" den Alpha-Wert (Wirkung der Änderung ) erhöhen
- mit r,g,b / R, G, B die Farbe justieren

- wenn fertig mit "w" die color config updaten (vorsicht, wird überschrieben!!!)

hope it works...

=====================================================================================

Erklärungen:
============

mode:
=====

add:              FB = aC + FB
subtract:         FB = aC - FB
subtract-reverse: FB = FB - aC

FB: FrameBuffer
aC: interpolated Vertex color (multiplied by alpha)

vertex position:

6==7==8
|  |  |
3==4==5
|  |  |
0==1==2


Argument: --color-correction XXX.xml


Tasten 
======

1-9: Auswahl vertex entsprechend 0-8

+  : additiver modus
-  : subtratik (VORSICHT!)
_  : reversesubtract, default: zieht Werte ab!

a  : alpha abschwächen (unterschiedliche Wirkung je nach mode)
A  : alpha verstärkern ( " )

r  : weniger Rot 
R  : mehr Rot 

g, G, b, B etc, etc., etc.

i  : vertex rücksetzen auf 1,1,1,0

w  : schreiben (name per --color-correction option!)



ein möglicher default mit mode reversesubtract:

=====================================================================================

<color-correction>
    <mode>
        <string>reversesubtract</string>
    </mode>

    <hsteps>
        <int>3</int>
    </hsteps>
    <vsteps>
        <int>3</int>
    </vsteps>

    <colorvalues>
        <!-- 0 -->
        <color>
            <red>
                <float>1.</float>
            </red>
            <green>
                <float>1.</float>
            </green>
            <blue>
                <float>1.</float>
            </blue>
            <alpha>
                <float>0.</float>
            </alpha>
        </color>

        <!-- 1 -->
        <color>
            <red>
                <float>1.</float>
            </red>
            <green>
                <float>1.</float>
            </green>
            <blue>
                <float>1.</float>
            </blue>
            <alpha>
                <float>0.</float>
            </alpha>
        </color>

        <!-- 2 -->
        <color>
            <red>
                <float>1.</float>
            </red>
            <green>
                <float>1.</float>
            </green>
            <blue>
                <float>1.</float>
            </blue>
            <alpha>
                <float>.0</float>
            </alpha>
        </color>

        <!-- 3 -->
        <color>
            <red>
                <float>1.</float>
            </red>
            <green>
                <float>1.</float>
            </green>
            <blue>
                <float>1.</float>
            </blue>
            <alpha>
                <float>.0</float>
            </alpha>
        </color>

        <!-- 4 -->
        <color>
            <red>
                <float>1.</float>
            </red>
            <green>
                <float>1.</float>
            </green>
            <blue>
                <float>1.</float>
            </blue>
            <alpha>
                <float>.0</float>
            </alpha>
        </color>

        <!-- 5 -->
        <color>
            <red>
                <float>1.</float>
            </red>
            <green>
                <float>1.</float>
            </green>
            <blue>
                <float>1.</float>
            </blue>
            <alpha>
                <float>.0</float>
            </alpha>
        </color>

        <!-- 6 -->
        <color>
            <red>
                <float>1.</float>
            </red>
            <green>
                <float>1.</float>
            </green>
            <blue>
                <float>1.</float>
            </blue>
            <alpha>
                <float>0.</float>
            </alpha>
        </color>

        <!-- 7 -->
        <color>
            <red>
                <float>1.</float>
            </red>
            <green>
                <float>1.</float>
            </green>
            <blue>
                <float>1.</float>
            </blue>
            <alpha>
                <float>.0</float>
            </alpha>
        </color>

        <!-- 8 -->
        <color>
            <red>
                <float>1.</float>
            </red>
            <green>
                <float>1.</float>
            </green>
            <blue>
                <float>1.</float>
            </blue>
            <alpha>
                <float>.0</float>
            </alpha>
        </color>

    </colorvalues>
    
</color-correction>


=====================================================================================

Die IntParam Werte in WaterAppModule:

IntParam:
=========


bit 0:  show ice/surface
bit 1:  generate cracks with splashes
bit 2:  reset water -> "mache wasser spiegelglatt"
bit 3:  setze dämpfung auf 0 : nur in gebieten wo splashes waren,
        gibt es wellen
bit 4:  zurücksetzen der dämpfung: Wellen überall
bit 5:  lasse wasser hereinrauschen...

wasser einstellbar in water-config.xml:

<cracksettings><waterdamping><float>0.9993
dämpfungswert des wassers...sehr empfindlich

<cracksettings><waterspeed><float>0.005
(default is 0.005 > 0.5% der Fläche pro frame bewässert)

<cracksettings><numwaterdrops><int>5
anzahl drops pro frame

<cracksettings><waterdropsize><float>1.
relative groesse einzelner drops

<cracksettings><waterdropintensity><float>1.
relative intensität einzelner drops



Werte für: (Werte, nicht bit-nummern)
----------

"kein wasser": 2 + 4 = 6

hereinlaufendes wasser: 32

übergang zu normalen wasser: 16

wasser mit splashes : 0

zufrieren 1: 1 (FloatParam erst auf 0)
zufrieren 2: 1 (FloatParam auf 0.95 erhöhen)

brechendes eis: 3

schmelzendes eis: 3 (FLoatParam gegen 0 gehen lassen)

wieder wasser: 0



=====================================================================================

