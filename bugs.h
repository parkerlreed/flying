#ifdef _bugs_h

Gefundene Fehler und Besonderheiten bei verschiedenen Compilern


BORLANDC
========

-  RealZero als Define (aus real.h) führt während der Übersetzung von Vec3.C
	zum Absturz des Compilers.

=> Statt Define muß Real-Konstante verwendet werden.


-  Es gibt Schwierigkeiten bei der Typdeklaration folgender
	(klassenspezifischer) Typen der Klassen Object und DynObj wegen
	der gleichen Namen.

	typedef void (Object::*ObjFun)();
	typedef void (DynObj::*ObjFun)();

=> Unterscheidung durch eindeutige Namen  DynObj::*ObjFun -> DynObj::*DynObjFun


-  Die Kombination friend inline führt in vec2.h und vec3.h zu Problemen.

=> Da Alternativen von gcc angemeckert werden, wird eine Unterscheidung
   über Schalter eingebaut.



TURBOC
======

- findet Fehler in DOSPBALL.H. Im Konstruktor fehlte die Initialisierung der
  display-Variable. -> eigentlich Fehler von BORLANDC



HP-CC
=====
-  kann Real als Klasse (Schalter REAL_IS_CLASS in global.h) nicht übersetzen,
	da bei einigen Operatoren scheinbar Mehrdeutigkeiten auftreten. Die 
   Mehrdeutigkeit entsteht bei Verwendung von Real und Integers in einem
   Ausdruck, da der Compiler dann nicht mehr zwischen Real/Real- und
   double/double-Operator entscheiden kann.

=> alle (absolut alle!) Kombinationen von Real- und int-Typen in Ausdruecken
   muessen explizit den int in einen Real casten !



-  purify meldet beim Matrizentest-Programm folgende Meldung:

|  ****  Purify'd check  (pid 29719)  ****
|  Purify (umr): uninitialized memory read:
|    * This is occurring while in:
|  	operator/(const Vector&,const Matrix&) [line 211, matrix.C, pc=0x28f44]
|  	main         [line 26, matrix_x.C, pc=0x2d6d8]
|  	_start       [libc.sl, pc=0x817ef04c]
|  	$START$      [check, pc=0x5740]
|    * Reading 4 bytes from 0x7b033630 on stack (2 bytes at 0x7b033632 uninit).
|    * This is local variable "x" in function main.
|  
|                           Chunks      Bytes
|                Leaked          0          0
|    Potentially Leaked          0          0
|                In-Use          7       4064
|    ----------------------------------------
|       Total Allocated          7       4064
|

=> ignorieren



GNU-CC
======
-  Version 2.5.7 kann in der double-Version von Real den impliziten Cast von
   Integer-Konstanten nach const Real& nicht korrekt durchführen.

	der Fehler führt zum Laufzeitabsturz !!!

=> Alle Integer-Konstanten, die als Real verwendet werden, müssen vorsorglich
	explizit als double in der Source auftauchen.


-  In xmover.C gibt es Schwierigkeiten bei der Macro-Ersetzung von 
	YADDOP zu + in der Zeile  c YADDOP= dy_c (also: c += dy_c)

=> c YADDOP dy_c verwenden und direkt += zuordnen


-  Version 2.6.0 hat Probleme mit dem Default-Copy-Konstruktor der
   Carrom-Klasse. Der Fehler tritt bei gleichzeitiger Verwendung eines
   mehrdimensionalen Arrays und eines einzelnen Objekts (im Beispiel Vec2)
   auf

=> Copy-Konstruktor wird explizit als dummy definiert


-  Real (*this) wird als konstant angenommen in
   inline Real Real::operator*( const Real &z ) const
		{ return (*this)*=z; }

=> Konstrukt variiert:

   inline Real Real::operator*( const Real &z ) const
		{ Real erg(*this); erg*=z; return erg; }


-  Version 2.4 hat Probleme mit dem protected member id der Klasse Object.
   Die public abgeleitete Klasse goal kann darauf nicht zugreifen.

=> id wird zum public member gemacht.


-  There is an optimization failure in gcc-2.7.0 which makes it impossible
   to update the queue on the screen, while it internally works correct.
   The problem occurs in the function PBallNorm::MoveQueue, where the new
   position of the queue should be set.

=> gcc-2.7.0 shouldn't be used with optimize -O2


FVWM
====

-  There is a struggle for the colormap, when running under fvwm.

=> The Server has to be grab, when the window manager deinstalles the map.


-  FVWM crashes when opening the window for debugging colors (using the
   option ShowColors)

=> Before opening the debug-window. The X-Server is synchronized and then
	a break for a second seems to help fvwm out. Don't ask me why?


allgemein
=========
-  Vector( int n, double, ... ); funktioniert nicht

=> ausgetauscht in Vector( int n, double d ), analog fuer Matrizen


#endif
