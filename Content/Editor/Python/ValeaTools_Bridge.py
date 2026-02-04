import unreal

try:
    ctx = observer_context
    unreal.log(f"ValeaTools geladen für Projekt: {ctx['project_name']} (Dev: {ctx['developer']})")
except NameError:
    unreal.log_error("Fehler: observer_context wurde von C++ nicht initialisiert!")
    ctx = {}

def init_observer_tools():
    # Hier könntest du z.B. automatisch deine Maya-Schnittstelle prüfen
    # oder die Wellen-Angriffe für den Extract Point im Editor validieren.
    unreal.log("Bereite dynamische Pfade für den Observer vor...")

if __name__ == "__main__":
    init_observer_tools()