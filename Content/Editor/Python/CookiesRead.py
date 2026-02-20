import unreal
import json
import os

def ReadCookies():

    psid = ""
    psidts = ""
    
    addon_dir = os.path.dirname(__file__)
    cookies_file = os.path.join(addon_dir, "cookies.json")
    
    unreal.log(f"Looking for: {cookies_file}")
    unreal.log(f"File exists: {os.path.exists(cookies_file)}")
    
    if os.path.exists(cookies_file):
        try:
            with open(cookies_file, 'r', encoding='utf-8') as f:
                data = json.load(f)
                
                if isinstance(data, list) and len(data) > 0:
                    cookie_data = data[0]
                else:
                    cookie_data = data
                
                psid = cookie_data.get("__Secure-1PSID", "").strip()
                psidts = cookie_data.get("__Secure-1PSIDTS", "").strip()
                
            unreal.log(f"Loaded from JSON:")
            unreal.log(f"   PSID Length: {len(psid)}")
            
            if psid:
                unreal.log(f"   PSID First 20: '{psid[:20]}'")
                unreal.log(f"   PSID Last 20: '{psid[-20:]}'")
            unreal.log(f"   PSIDTS Length: {len(psidts)}")
            
        except Exception as e:
            unreal.log(f"ERROR reading JSON: {e}")
    else:
        unreal.log("cookies.json not found!")
        
    return psid, psidts