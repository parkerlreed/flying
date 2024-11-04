// ============================================================================
const char *flying_descriptor_string =
	"@(#)Game of Pool/Snooker/Carrom/Hockey/Curling for X-Windows\n";
const char *flying_version_string =
	"@(#)$Revision: 6.20 $ $Date: 1995/10/03 13:26:57 $ Author: Helmut Hoenig\n";
static const char *revision = "$Revision: 6.20 $";
// ============================================================================
// $Log: logfile.h,v $
// Revision 6.20  1995/10/03  13:26:57  helmut
// *** empty log message ***
//
// Revision 6.19  1995/08/07  21:20:44  helmut
// update to gcc-2.7.0
//
// Revision 6.18  1995/07/26  21:57:05  helmut
// Update der Versions-Nummer wegen verlorenen Versionen
//
// Revision 6.16  1995/07/26  21:52:28  helmut
// Linux-Version
//
// Revision 6.18  1995/07/18  20:37:16  helmut
// minor correction for HP-UX
//
// Revision 6.17  1995/07/06  22:11:50  helmut
// Linux-Version for S.u.S.E.
//
// Revision 6.16  1995/07/06  20:33:05  helmut
// Optimized for Linux
//
// Revision 6.16  1995/07/06  20:33:05  helmut
// Optimized for Linux
//
// Revision 6.15  95/05/24  12:51:48  12:51:48  emshh (Helmut Hoenig)
// Korrektur Verbesserung: Interpixel-Motion
// 
// Revision 6.14  95/05/11  13:29:14  13:29:14  emshh (Helmut Hoenig)
// Korrektur: Interpixel-Motion
// 
// Revision 6.13  95/05/08  18:41:37  18:41:37  emshh (Helmut Hoenig)
// PBallNorm::WaitForEvents to reduce CPU-usage / 3rd Enhanced XMover
// 
// Revision 6.12  95/05/05  13:12:01  13:12:01  emshh (Helmut Hoenig)
// Enhanced XMover / Interpixel-Motion
// 
// Revision 6.11  95/03/14  13:29:28  13:29:28  emshh (Helmut Hoenig)
// neue Flags: -nohints, -red, -green, -blue / usleep_local
// 
// Revision 6.10  95/01/18  13:12:13  13:12:13  emshh (Helmut Hoenig)
// double buffered queue
// 
// Revision 6.9  94/12/20  09:04:09  09:04:09  emshh (Helmut Hoenig)
// Abgabe-Version
// 
// Revision 6.8  94/12/15  09:23:58  09:23:58  emshh (Helmut Hoenig)
// Font-Test / getenv-Test
// 
// Revision 6.7  94/12/14  13:14:07  13:14:07  emshh (Helmut Hoenig)
// Static Colors, neue Farben, weniger ANSI
// 
// Revision 6.6  94/12/02  12:51:03  12:51:03  emshh (Helmut Hoenig)
// neuer Header stdinc
// 
// Revision 6.5  94/11/24  13:05:12  13:05:12  emshh (Helmut Hoenig)
// neue Baelle
// 
// Revision 6.4  94/11/23  09:16:51  09:16:51  emshh (Helmut Hoenig)
// RAND_MASK f¸r zuf‰llige Hintergrundmuster
// 
// Revision 6.3  94/11/18  12:52:47  12:52:47  emshh (Helmut Hoenig)
// Portabler Code?
// 
// Revision 6.1  94/11/09  12:52:17  12:52:17  emshh (Helmut Hoenig)
// Version fuer gcc-2.6.0 / Ausgaben w‰hrend Pixmap-Berechnung
// 
// Revision 6.0  94/11/04  13:26:45  13:26:45  emshh (Helmut Hoenig)
// PD-version f¸r beta-Test
// 
// Revision 5.7  94/11/03  13:41:53  13:41:53  emshh (Helmut Hoenig)
// B‰lle aus Pocket zur¸ckholbar / Farben bei Reset 'r' korrekt
// 
// Revision 5.6  94/11/02  13:25:34  13:25:34  emshh (Helmut Hoenig)
// R¸cksetzen ohne ‹berlappung / neuer Einwurf: Hockey
// 
// Revision 5.5  94/09/24  09:14:07  09:14:07  emshh (Helmut Hoenig)
// Curling auch mit Keeper
// 
// Revision 5.4  94/09/23  13:25:23  13:25:23  emshh (Helmut Hoenig)
// Keeper als zeichenbares Objekt
// 
// Revision 5.3  94/09/19  13:31:44  13:31:44  emshh (Helmut Hoenig)
// Mehrfachvererbung: StaticArc - Pocket - Keeper
// 
// Revision 5.2  94/09/16  13:34:23  13:34:23  emshh (Helmut Hoenig)
// Curling 1
// 
// Revision 5.1  94/09/14  09:42:06  09:42:06  emshh (Helmut Hoenig)
// PullMove in Billard / Default-Ballpositionen / englischer Header
// 
// Revision 5.0  94/05/13  07:14:46  07:14:46  emshh (Helmut Hoenig)
// Vollversion / ISO-Fonts
// 
// Revision 4.15  94/03/18  13:10:36  13:10:36  emshh (Helmut Hoenig)
// alle int->double casts explizit
// 
// Revision 4.14  94/03/10  13:31:31  13:31:31  emshh (Helmut Hoenig)
// allgemeine Vektor-Klasse
// 
// Revision 4.13  94/03/08  13:25:23  13:25:23  emshh (Helmut Hoenig)
// Klasse Vector->Vec2
// 
// Revision 4.12  94/02/09  13:30:52  13:30:52  emshh (Helmut Hoenig)
// korrekte Rotation
// 
// Revision 4.11  94/02/07  13:27:41  13:27:41  emshh (Helmut Hoenig)
// Redraw
// 
// Revision 4.10  94/02/03  13:25:49  13:25:49  emshh (Helmut Hoenig)
// komprimieren der RingState-Daten
// 
// Revision 4.9  94/02/02  12:59:54  12:59:54  emshh (Helmut Hoenig)
// RingState-Fehler behoben / right/left nicht abspeichern
// 
// Revision 4.8  94/01/28  14:08:38  14:08:38  emshh (Helmut Hoenig)
// Rundungsfehlerverfolgung in xmover.C
// 
// Revision 4.7  94/01/26  13:06:07  13:06:07  emshh (Helmut Hoenig)
// Portierung auf GNU-CC fertig
// 
// Revision 4.6  94/01/25  13:22:23  13:22:23  emshh (Helmut Hoenig)
// RealZero Vec2Zero Vec3Zero eingefuegt
// 
// Revision 4.5  94/01/25  09:30:18  09:30:18  emshh (Helmut Hoenig)
// Queue Locked after Shooting
// 
// Revision 4.4  94/01/21  13:19:55  13:19:55  emshh (Helmut Hoenig)
// GNU-Fehler int auf const &Real implizit casten
// 
// Revision 4.3  94/01/18  12:47:55  12:47:55  emshh (Helmut Hoenig)
// purified / GNU-CC
// 
// Revision 4.2  94/01/05  13:10:38  13:10:38  emshh (Helmut Hoenig)
// Caching der Berechneten Bilder in Dateien
// 
// Revision 4.1  93/12/21  14:50:12  14:50:12  emshh (Helmut Hoenig)
// Cueball in Demo / Destruktoren in Carrom
// 
// Revision 4.0  93/12/21  12:54:42  12:54:42  emshh (Helmut Hoenig)
// Hockey (2) mit Pointer
// 
// Revision 3.5  93/12/21  11:03:35  11:03:35  emshh (Helmut Hoenig)
// Deluxe (2) - Rollende Kugeln in alle Richtungen
// 
// Revision 3.4  93/12/16  13:25:04  13:25:04  emshh (Helmut Hoenig)
// Vec3-Rotationen / PBall-Type
// 
// Revision 3.3  93/12/15  09:18:06  09:18:06  emshh (Helmut Hoenig)
// Deluxe (1) - Rollende Kugeln in X-Richtung
// 
// Revision 3.2  93/12/13  13:24:55  13:24:55  emshh (Helmut Hoenig)
// Hockey (1) / HalfBallMover (1) / Test-Klasse / interne Farben
// 
// Revision 3.1  93/12/10  13:23:49  13:23:49  emshh (Helmut Hoenig)
// OFFSET-Korrektur / Destruktoren
// 
// Revision 3.0  93/12/09  13:27:16  13:27:16  emshh (Helmut Hoenig)
// Konfi-Datei
// 
// Revision 2.14  93/12/09  09:09:01  09:09:01  emshh (Helmut Hoenig)
// Snooker & Pool-Demo / Spielfeldmarkierungen / DOS-Farben
// 
// Revision 2.13  93/12/08  13:04:56  13:04:56  emshh (Helmut Hoenig)
// Statistik unten / Variablen in Modul global.C
// 
// Revision 2.12  93/12/07  13:39:57  13:39:57  emshh (Helmut Hoenig)
// Carrom-Demo / Statistik-Funktion
// 
// Revision 2.11  93/12/06  13:18:15  13:18:15  emshh (Helmut Hoenig)
// DiscMover
// 
// Revision 2.10  93/12/06  09:37:58  09:37:58  emshh (Helmut Hoenig)
// Carrom (X) / Klasse ShadedMover
// 
// Revision 2.9  93/12/06  07:30:53  07:30:53  emshh (Helmut Hoenig)
// Carrom / Mover-Aufsplittung / Mischfarben
// 
// Revision 2.8  93/12/03  17:32:51  17:32:51  emshh (Helmut Hoenig)
// Log-Datei
// 
// Revision 2.7  93/12/03  14:56:44  14:56:44  emshh (Helmut Hoenig)
// Ausgabe der Rate korrigiert
// 
// Revision 2.6  93/12/03  13:04:14  13:04:14  emshh (Helmut Hoenig)
// erweiterte Beleuchtung / Trace-Windows / XArc-Fehler umgangen
// 
// Revision 2.5  93/12/02  13:21:34  13:21:34  emshh (Helmut Hoenig)
// Pocketline am unteren Bildrand
// 
// Revision 2.4  93/12/02  11:04:53  11:04:53  emshh (Helmut Hoenig)
// Optimierung der Include's
// 
// Revision 2.3  93/12/02  09:42:23  09:42:23  emshh (Helmut Hoenig)
// Tischmaﬂe und Tascheneinlauf fertig
// 
// Revision 2.2  93/12/02  07:31:19  07:31:19  emshh (Helmut Hoenig)
// DOS-Farben / Tischmaﬂe in cm / gerundeter Tascheneinlauf (1)
// 
// Revision 2.1  93/12/01  09:34:20  09:34:20  emshh (Helmut Hoenig)
// Neue Farben
// 
// Revision 2.0  93/11/30  14:23:24  14:23:24  emshh (Helmut Hoenig)
// Initialversion: Spielbezogene Farbwahl
// 
// ============================================================================
