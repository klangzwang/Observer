import unreal
import json
import os

def SaveCookies(psid_ref, psidts_ref):
    
    """Save cookies to cookies.json file"""
    # bl_idname = "nanobanana.save_cookies"
    # bl_label = "Save Cookies"
    
    unreal.log("SAVE COOKIES DEBUG")

    # Step 1: Read raw values from UI fields
    psid = psid_ref.get_text() or ""
    psidts = psidts_ref.get_text() or ""
    
    unreal.log(f"1. RAW from UI Field:")
    unreal.log(f"   PSID Length: {len(psid)}")
    unreal.log(f"   PSID First 20: '{psid[:20]}'")
    unreal.log(f"   PSID Last 20: '{psid[-20:]}'")
    unreal.log(f"   PSIDTS Length: {len(psidts)}")
    
    script_dir = os.path.dirname(ValeaAIRenderer)
    cookies_file = os.path.join(script_dir, "cookies.json")
    
    cookies_data = {
        "__Secure-1PSID": psid,
        "__Secure-1PSIDTS": psidts
    }
    
    unreal.log(f"2. WRITING to: {cookies_file}")
    unreal.log(f"   Data to write: PSID={len(psid)} chars, PSIDTS={len(psidts)} chars")
    
    try:
        with open(cookies_file, 'w', encoding='utf-8') as f:
            json.dump(cookies_data, f, indent=4)
        
        with open(cookies_file, 'r', encoding='utf-8') as f:
            verify_data = json.load(f)
            verify_psid = verify_data.get("__Secure-1PSID", "")
            verify_psidts = verify_data.get("__Secure-1PSIDTS", "")
        
        unreal.log(f"3. VERIFICATION (re-read file):")
        unreal.log(f"   PSID Length: {len(verify_psid)}")
        unreal.log(f"   PSID First 20: '{verify_psid[:20]}'")
        unreal.log(f"   PSID Last 20: '{verify_psid[-20:]}'")
        unreal.log(f"   Match: {verify_psid == psid}")
        
        if len(verify_psid) != len(psid):
            unreal.log(f"   WARNING: Written length ({len(verify_psid)}) != Input length ({len(psid)})")

    except Exception as e:
        unreal.log(f"ERROR: {e}")

def setup_cookies(psid_text):
    
    psid_text_string = psid_text
    
    unreal.log(f"PSIDText gefunden: {psid_text_string}")
