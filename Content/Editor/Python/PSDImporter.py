import unreal
import os
import tkinter as tk
from tkinter import filedialog
from psd_tools import PSDImage

def get_psd_path():
    """Öffnet einen nativen Windows-Dialog ohne Unreal-API-Hürden."""
    root = tk.Tk()
    root.withdraw()
    root.attributes('-topmost', True)
    path = filedialog.askopenfilename(
        title="Observer: PSD auswählen",
        filetypes=[("Photoshop Dateien", "*.psd")]
    )
    root.destroy()
    return path

def create_ui_material(texture_asset, layer_name, dest_path):
    """Erstellt ein UI-Material für den Layer."""
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    mat_name = f"M_{layer_name}"
    
    # Material erstellen
    material_asset = asset_tools.create_asset(mat_name, dest_path, unreal.Material, unreal.MaterialFactoryNew())
    
    # Einstellungen für UI
    material_asset.set_editor_property('material_domain', unreal.MaterialDomain.DOMAIN_USER_INTERFACE)
    material_asset.set_editor_property('blend_mode', unreal.BlendMode.BLEND_TRANSLUCENT)
    
    # Texture Sampler hinzufügen
    ts_node = unreal.MaterialEditingLibrary.create_material_expression(material_asset, unreal.MaterialExpressionTextureSample, -400, 0)
    ts_node.set_editor_property('texture', texture_asset)
    
    # Verbinden
    unreal.MaterialEditingLibrary.connect_material_expressions(ts_node, "RGB", material_asset, "Final Color")
    unreal.MaterialEditingLibrary.connect_material_expressions(ts_node, "A", material_asset, "Opacity")
    
    unreal.MaterialEditingLibrary.recompile_material(material_asset)

def run_psd_import():
    psd_path = get_psd_path()
    if not psd_path:
        return

    psd = PSDImage.open(psd_path)
    dest_path = "/Game"
    
    # Deine spezifischen Layer-Namen
    targets = ["Header", "Button1", "Button2"]
    
    # Progress Bar im Editor
    with unreal.ScopedSlowTask(len(targets), "Extrahiere Layer...") as slow_task:
        slow_task.make_dialog(True)

        for layer in psd.descendants():
            if layer.name in targets:
                slow_task.enter_progress_frame(1, f"Importiere {layer.name}...")
                
                # PNG Export
                temp_file = os.path.join(os.environ["TEMP"], f"temp_{layer.name}.png")
                layer.composite().save(temp_file)
                
                # Import Task
                task = unreal.AssetImportTask()
                task.set_editor_property('automated', True)
                task.set_editor_property('destination_name', layer.name)
                task.set_editor_property('destination_path', dest_path)
                task.set_editor_property('filename', temp_file)
                task.set_editor_property('replace_existing', True)
                task.set_editor_property('save', True)
                
                unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
                
                # Material erstellen
                tex = unreal.EditorAssetLibrary.load_asset(f"{dest_path}/{layer.name}")
                if tex:
                    create_ui_material(tex, layer.name, dest_path)
                
                os.remove(temp_file)

    unreal.log("PSD Import erfolgreich!")