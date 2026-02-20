import unreal
import CookiesRead

import urllib.request
import urllib.parse
import json
import time
import os
import re
import ssl

_AGENT_LOG_PATH = r"c:\debug.log"

class StartClient:
    def __init__(self):
        psid, psidts = CookiesRead.ReadCookies()
        psid = (psid or "").strip()
        psidts = (psidts or "").strip()

        unreal.log((psid or "").strip())
        unreal.log((psidts or "").strip())

        self.cookies = {"__Secure-1PSID": psid}
        if psidts:
            self.cookies["__Secure-1PSIDTS"] = psidts
        
        self.cookie_str = "; ".join([f"{k}={v}" for k, v in self.cookies.items()])
        self.snlm0e = None
        self.ssl_context = ssl.create_default_context()
        self.log_callback = None
        
        
        
        
        
        
        unreal.log({self.cookie_str})
        
        self.token = StartClient.get_snlm0e(self)
        
        try:
            with open(_AGENT_LOG_PATH, "a", encoding="utf-8") as _f:
                _f.write(json.dumps({
                    "sessionId": "debug-session",
                    "runId": "re-port",
                    "location": "StartClient.py:__init__",
                    "message": "StartClient init with updated logic",
                    "timestamp": int(time.time() * 1000)
                }) + "\n")
        except Exception:
            pass
            
    def log(self, msg):
        """Log to both console and UI if callback is set"""
        unreal.log(f"[GeminiAPI] {msg}")
        if self.log_callback:
            self.log_callback(msg)

    def get_snlm0e(self):
        """Extracts SNlM0e token"""
        # Try /app first, then root /
        urls = ["https://gemini.google.com/app", "https://gemini.google.com/"]
        
        for url in urls:
            headers = {
                "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
                "Cookie": self.cookie_str,
                "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
            }
            
            req = urllib.request.Request(url, headers=headers)
            
            try:
                self.log(f"Contacting {url}...")
                response = urllib.request.urlopen(req, context=self.ssl_context, timeout=30)
                
                final_url = response.geturl()
                self.log(f"Final URL: {final_url}")
                
                if "accounts.google.com" in final_url or "ServiceLogin" in final_url:
                    self.log("❌ REDIRECTED TO LOGIN. Cookies are invalid/expired.")
                    continue
                
                status = response.status
                if status != 200:
                    self.log(f"Bad status: {status}")
                    continue
                    
                html = response.read().decode('utf-8')
                
                # Check for SNlM0e
                # Variant 1: Standard key
                match = re.search(r'SNlM0e":"(.*?)"', html)
                if match:
                    self.snlm0e = match.group(1)
                    self.log(f"Token OK: {self.snlm0e[:8]}...")
                    return self.snlm0e
                
                # Variant 2: Escaped quote?
                match = re.search(r'SNlM0e\\":\\"(.*?)\\"', html)
                if match:
                    self.snlm0e = match.group(1)
                    self.log(f"Token OK (Var2): {self.snlm0e[:8]}...")
                    return self.snlm0e
                    
                self.log("Token pattern NOT FOUND on this URL.")
                
            except Exception as e:
                self.log(f"Error checking {url}: {str(e)}")
        
        self.log("Failed to get token from any EP.")
        return None