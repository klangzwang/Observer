import unreal
import json
import os
import gemini_api
import tempfile
import time

def add_log(message, log_type="info"):
    print(f"[AIRenderer] {message}")

def StartGenerate(prompt, image_path, psid, psidts):

    try:
        client = gemini_api.GeminiClient(psid, psidts)
        client.log_callback = lambda msg: add_log(msg) 
        
        unreal.log("Checking Authentication...")
        token = client.get_snlm0e()
        
        if not token:
            unreal.log_error("Auth Failed. Check cookies/VPN.")
            # processing_state.is_processing = False
            return
        
        if client:
            unreal.log("Client found...")
        
        unreal.log("Auth OK. Uploading image...")
        text, image_urls = client.send_prompt(prompt, image_path)
        if text is None:
            unreal.log_error("No response from Gemini")
            return

        if text:
            add_log(f"{text[:60]}...")
        
        if image_urls:
            unreal.log(f"Downloading {len(image_urls)} image(s)...")
            url = image_urls[0]
            
            temp_dir = tempfile.gettempdir()
            filename = f"airenderer_{int(time.time())}.png"
            source_path = os.path.join(temp_dir, filename)

            destination_path = "/Game"
            
            if client.download_file(url, source_path):
                unreal.log("Downloaded!")

                task = unreal.AssetImportTask()
                task.set_editor_property('filename', source_path)
                task.set_editor_property('destination_path', destination_path)
                task.set_editor_property('automated', True)           # Das verhindert Dialoge
                task.set_editor_property('replace_existing', True)    # Überschreibt falls vorhanden
                task.set_editor_property('save', True)                # Speichert das Asset sofort
                
                asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
                asset_tools.import_asset_tasks([task])
            else:
                unreal.log_error("Download failed")
        else:
            unreal.log_error("No generated images found")


    except Exception as e:
        unreal.log_error(f"Exception: {e}")
        import traceback
        traceback.print_exc()
