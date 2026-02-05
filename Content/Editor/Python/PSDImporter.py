import unreal
import os
import tkinter as tk
from tkinter import filedialog
from psd_tools import PSDImage

def get_psd_path():
    """Öffnet den Windows-Dialog zur Dateiauswahl."""
    root = tk.Tk()
    root.withdraw()
    root.attributes('-topmost', True)
    path = filedialog.askopenfilename(
        title="PSD für Projekt Observer auswählen",
        filetypes=[("Photoshop Dateien", "*.psd")]
    )
    root.destroy()
    return path

def run_psd_import():
    psd_path = get_psd_path()
    if not psd_path:
        return

    psd = PSDImage.open(psd_path)
    dest_path = "/Game/UI/Observer_Assets"
    
    # Sicherstellen, dass das Zielverzeichnis existiert
    if not unreal.EditorAssetLibrary.does_directory_exist(dest_path):
        unreal.EditorAssetLibrary.make_directory(dest_path)

    # Array zum Sammeln aller gefundenen Layernamen
    found_layer_names = []
    
    # Alle Layer holen
    all_layers = list(psd.descendants())
    
    # Progress Bar
    with unreal.ScopedSlowTask(len(all_layers), "Verarbeite PSD Layer...") as slow_task:
        slow_task.make_dialog(True)

        for layer in all_layers:
            # 1. Sicherstellen, dass der Layer sichtbar ist
            if not layer.is_visible():
                continue
                
            # 2. Prüfen, ob der Layer tatsächlich Bilddaten enthält (keine leere Gruppe o.ä.)
            try:
                # Versuchen, das Bild des Layers zu rendern
                layer_image = layer.composite()
            except Exception:
                # Wenn das fehlschlägt, ist es wahrscheinlich ein Container/Gruppe ohne Inhalt
                continue
                
            layer_name = layer.name
            found_layer_names.append(layer_name)
            
            slow_task.enter_progress_frame(1, f"Importiere: {layer_name}")
            
            # Als temporäres PNG exportieren
            temp_file = os.path.join(os.environ["TEMP"], f"temp_{layer_name}.png")
            layer_image.save(temp_file)
            
            # Import Task
            task = unreal.AssetImportTask()
            task.set_editor_property('automated', True)
            task.set_editor_property('destination_name', layer_name)
            task.set_editor_property('destination_path', dest_path)
            task.set_editor_property('filename', temp_file)
            task.set_editor_property('replace_existing', True)
            task.set_editor_property('save', True)
            
            unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
            
            # Temp Datei löschen
            os.remove(temp_file)

    unreal.log(f"Import abgeschlossen. Gefundene Layer: {found_layer_names}")