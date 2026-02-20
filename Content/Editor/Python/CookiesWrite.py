import unreal
import json
import os

def WriteCookies(psid, psidts):
    
    addon_dir = os.path.dirname(__file__)
    cookies_file = os.path.join(addon_dir, "cookies.json")

    cookies_data = [
        {
            "Name": "Unset",
            "__Secure-1PSID": psid,
            "__Secure-1PSIDTS": psidts
        }
    ]

    try:
        with open(cookies_file, 'w', encoding='utf-8') as f:
            json.dump(cookies_data, f, indent=4)
    except Exception as e:
        unreal.log(f"ERROR: {e}")