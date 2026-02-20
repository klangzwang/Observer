import unreal
import os
import tempfile
import threading

def capture_active_view_to_disk():
    """
    Erfasst die Ansicht und speichert sie als PNG. 
    Verwendet den allgemeinsten Konstruktor für Objekte.
    """
    
    # 1. Setup Paths
    temp_dir = tempfile.gettempdir()
    filename = f"unreal_observer_capture_{os.getpid()}.png"
    filepath = os.path.join(temp_dir, filename)

    level_library = unreal.EditorLevelLibrary()
    
    # 2. Kamera finden
    selected_actors = level_library.get_selected_level_actors()
    target_camera = None
    
    for actor in selected_actors:
        if isinstance(actor, unreal.CameraActor):
            target_camera = actor
            break
            
    viewport_loc = unreal.Vector(0,0,0)
    viewport_rot = unreal.Rotator(0,0,0)
    
    if target_camera:
        viewport_loc = target_camera.get_actor_location()
        viewport_rot = target_camera.get_actor_rotation()
    else:
        # Fallback auf Viewport
        viewport_client = unreal.EditorLevelLibrary.get_level_viewport_camera_info()
        viewport_loc = viewport_client[0]
        viewport_rot = viewport_client[1]

    # 3. Configure for Capture
    temp_actor = level_library.spawn_actor_from_class(unreal.SceneCapture2D, viewport_loc, viewport_rot)
    
    if not temp_actor:
        print("Error: Konnte SceneCapture Actor nicht spawnen.")
        return None

    try:
        capture_component = temp_actor.get_component_by_class(unreal.SceneCaptureComponent2D)
        capture_component.capture_source = unreal.SceneCaptureSource.SCS_FINAL_COLOR_LDR
        
        # --- ROBUSTER ANSATZ: Construct Object ---
        # Wir erzeugen das Objekt generisch
        rt = unreal.new_object(unreal.TextureRenderTarget2D)
        
        # Jetzt setzen wir die Properties, ABER wir müssen update_resource() 
        # aufrufen, DAMIT die Einstellungen übernommen werden, bevor wir es zuweisen.
        rt.set_editor_property("size_x", 1920)
        rt.set_editor_property("size_y", 1080)
        rt.set_editor_property("render_target_format", unreal.TextureRenderTargetFormat.RTF_RGBA8)
        
        # Die Methode update_resource() ist essenziell. 
        # Sollte sie *wirklich* fehlen, benutzen wir einen Workaround.
        try:
            rt.update_resource()
        except AttributeError:
            print("Warning: update_resource() fehlt, versuche Zuweisung direkt.")
        
        capture_component.texture_target = rt
        # --- ROBUSTER ANSATZ ENDE ---
        
        if target_camera:
             capture_component.fov_angle = target_camera.camera_component.field_of_view
        
        print(f"Capturing view to {filepath}...")
        
        # 4. Render
        capture_component.capture_scene()
        
        # 5. Export to Disk
        write_options = unreal.ImageWriteOptions()
        write_options.format = unreal.DesiredImageFormat.PNG
        write_options.overwrite_file = True
        write_options.async_ = False 
        
        unreal.ImageWriteBlueprintLibrary.export_to_disk(rt, filepath, write_options)
        
        if os.path.exists(filepath):
            print(f"Success: Saved to {filepath}")
            return filepath
        else:
            print("Error: Datei konnte nicht erstellt werden.")
            return None

    except Exception as e:
        print(f"Render failed: {e}")
        return None
        
    finally:
        # 6. Cleanup
        if temp_actor:
            level_library.destroy_actor(temp_actor)
            
    return filepath
    
#if __name__ == "__main__":
#    capture_active_view_to_disk()