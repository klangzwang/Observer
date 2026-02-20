import unreal
import json
import os
import gemini_api

def RotateCookies():

    psid = ""
    psidts = ""
    
    addon_dir = os.path.dirname(__file__)
    cookies_file = os.path.join(addon_dir, "cookies.json")
    
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
            
        except Exception as e:
            unreal.log(f"ERROR reading JSON: {e}")
    else:
        unreal.log("cookies.json not found!")

    return gemini_api.rotate_cookies(psid, psidts)