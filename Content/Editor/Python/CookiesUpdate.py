import subprocess
import os
import tempfile
import browser_cookie3
import unreal

def UpdateCookies():
    
    addon_dir = os.path.dirname(__file__)
    cookies_file = os.path.join(addon_dir, "cookies.json")

    try:
        with open(cookies_file, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        psid = data.get("__Secure-1PSID", "").strip()
        psidts = data.get("__Secure-1PSIDTS", "").strip()        

    except Exception as e:
        unreal.log(f"ERROR: {e}")

def get_psid():
    
    appdata = os.getenv('LOCALAPPDATA')
    # WICHTIG: Prüfe ob dein Profil wirklich 'Default' ist oder 'Profile 1'
    source_dir = os.path.join(appdata, r"Google\Chrome\User Data\Default\Network")
    cookie_file = "Cookies"
    
    temp_dir = tempfile.gettempdir()
    temp_cookie_path = os.path.join(temp_dir, "temp_cookies")

    try:
        # Robocopy ist resistenter gegen Dateisperren
        # /R:0 = Keine Wiederholungen bei Fehler
        # /W:0 = Keine Wartezeit
        subprocess.run(['robocopy', source_dir, temp_dir, cookie_file, '/R:0', '/W:0'], shell=True)
        
        # Falls robocopy die Datei unter anderem Namen abgelegt hat
        final_temp_path = os.path.join(temp_dir, cookie_file)

        if os.path.exists(final_temp_path):
            cj = browser_cookie3.chrome(cookie_file=final_temp_path, domain_name='.youtube.com')
            for cookie in cj:
                if cookie.name == '__Secure-1PSID':
                    # Aufräumen
                    os.remove(final_temp_path)
                    return str(cookie.value)
        
        return "COOKIE_NOT_FOUND"

    except Exception as e:
        unreal.log_error(f"BruteForce Error: {e}")
        return "STILL_LOCKED"

def UpdatePsidCookie():

    try:
        cj = browser_cookie3.chrome(domain_name='.youtube.com')
        
        psid = "notfound"
        
        for cookie in cj:
            if cookie.name == '__Secure-1PSID':
                psid = str(cookie.value)
                break

        if psid:
            unreal.log(f"Erfolg! Cookie-Wert: {psid}")
        else:
            unreal.log("Cookie '__Secure-1PSID' wurde nicht gefunden.")

    except Exception as e:
        print(f"Fehler beim Auslesen: {e}")
        psid = "ERROR"
        
def UpdatePsidTsCookie():

    try:
        cj = browser_cookie3.chrome(domain_name='.youtube.com')
        
        psidts = "notfound"
        
        for cookie in cj:
            if cookie.name == '__Secure-1PSIDTS':
                psidts = str(cookie.value)
                break

        if psidts:
            unreal.log(f"Erfolg! Cookie-Wert: {psidts}")
        else:
            unreal.log("Cookie '__Secure-1PSIDTS' wurde nicht gefunden.")

    except Exception as e:
        print(f"Fehler beim Auslesen: {e}")
        psidts = "ERROR"