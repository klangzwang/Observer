import urllib.request
import urllib.parse
import unreal
import json
import time
import os
import re
import ssl

# Debug-mode log path (NDJSON, one JSON object per line)
_AGENT_LOG_PATH = r"c:\debug.log"

class GeminiClient:
    def __init__(self, prompt, image_path, psid, psidts=None):
        psid = (psid or "").strip()
        psidts = (psidts or "").strip()
        
        self.cookies = {"__Secure-1PSID": psid}
        if psidts:
            self.cookies["__Secure-1PSIDTS"] = psidts
        
        self.cookie_str = "; ".join([f"{k}={v}" for k, v in self.cookies.items()])
        self.snlm0e = None
        self.ssl_context = ssl.create_default_context()
        
        # Callback for logging (set by caller)
        self.log_callback = None

        #
        # Start Generating
        #
        token = self.get_snlm0e()
        
        if not token:
            unreal.log_error("Auth Failed. Check cookies/VPN.")
            return

        unreal.log("Auth OK. Uploading image...")
        
        text, image_urls = self.send_prompt(prompt, image_path)
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
        
        # End Generating

        # #region agent log
        try:
            with open(_AGENT_LOG_PATH, "a", encoding="utf-8") as _f:
                _f.write(json.dumps({
                    "sessionId": "debug-session",
                    "runId": "re-port",
                    "location": "gemini_api.py:__init__",
                    "message": "GeminiClient init with updated logic",
                    "timestamp": int(time.time() * 1000)
                }) + "\n")
        except Exception:
            pass
        # #endregion agent log

    def log(self, msg):
        """Log to both console and UI if callback is set"""
        print(f"[GeminiAPI] {msg}")
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

    def upload_image(self, image_path):
        """Uploads image to content-push.googleapis.com"""
        url = "https://content-push.googleapis.com/upload"
        
        if not os.path.exists(image_path):
            self.log(f"Image not found: {image_path}")
            return None

        file_size = os.path.getsize(image_path)
        with open(image_path, 'rb') as f:
            image_data = f.read()
            
        # Determine content type
        ext = os.path.splitext(image_path)[1].lower()
        content_type = 'image/png'
        if ext in ['.jpg', '.jpeg']:
            content_type = 'image/jpeg'
        elif ext == '.webp':
            content_type = 'image/webp'

        headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
            "Cookie": self.cookie_str,
            "Push-ID": "feeds/mcudyrk2a4khkz",
            "Content-Type": content_type,
            "X-Goog-Upload-Protocol": "raw",
            "X-Goog-Upload-Header-Content-Length": str(file_size),
            "X-Goog-Upload-Header-Content-Type": content_type
        }
        
        req = urllib.request.Request(url, data=image_data, headers=headers, method='POST')
        
        try:
            self.log(f"Uploading ({file_size} bytes)...")
            response = urllib.request.urlopen(req, context=self.ssl_context, timeout=60)
            if response.status == 200:
                result = response.read().decode('utf-8').strip()
                self.log("Upload OK")
                return result
            else:
                self.log(f"Upload failed: {response.status}")
                return None
        except Exception as e:
            self.log(f"Upload error: {str(e)}")
            return None

    def send_prompt(self, prompt, image_path=None):
        if not self.snlm0e:
            if not self.get_snlm0e():
                return None, None
            
        url = "https://gemini.google.com/_/BardChatUi/data/assistant.lamda.BardFrontendService/StreamGenerate"
        
        file_upload_result = None
        if image_path:
            file_upload_result = self.upload_image(image_path)
            
        self.log({file_upload_result})
            
        files_structure = []
        if file_upload_result:
            filename = os.path.basename(image_path)
            files_structure = [[[file_upload_result], filename]]
            
        # Payload construction matching Ruby exactly
        prompt_data = [prompt, 0, None, files_structure if files_structure else None, None, None, 0]
        inner_payload = [prompt_data, None, None]
        
        inner_json = json.dumps(inner_payload)
        f_req = json.dumps([None, inner_json])
        
        data = urllib.parse.urlencode({"at": self.snlm0e, "f.req": f_req}).encode('utf-8')
        
        headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
            "Cookie": self.cookie_str,
            "Content-Type": "application/x-www-form-urlencoded;charset=utf-8",
            "Origin": "https://gemini.google.com",
            "Referer": "https://gemini.google.com/",
            "X-Same-Domain": "1",
            "x-goog-ext-525001261-jspb": '[1,null,null,null,"9d8ca3786ebdfbea",null,null,0,[4],null,null,2]'
        }
        
        req = urllib.request.Request(url, data=data, headers=headers, method='POST')

        try:
            self.log("Sending to Gemini...")
            response = urllib.request.urlopen(req, context=self.ssl_context, timeout=60)
            if response.status == 200:
                body = response.read().decode('utf-8')
                return self.parse_response(body)
            else:
                self.log(f"Gemini error: {response.status}")
                return None, None
        except Exception as e:
            self.log(f"Request error: {str(e)}")
            return None, None

    def get_full_res_url(self, image_id):
        """Fetch high-res URL from library (RPC jGArJ)"""
        url = "https://gemini.google.com/_/BardChatUi/data/assistant.lamda.BardFrontendService/jGArJ"
        
        # Payload for jGArJ
        inner_payload = [[["jGArJ", "[[1,1,1,1,1,0,1],30]", None, "generic"]]]
        f_req = json.dumps(inner_payload)
        
        data = urllib.parse.urlencode({"at": self.snlm0e, "f.req": f_req}).encode('utf-8')
        
        headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
            "Cookie": self.cookie_str,
            "Content-Type": "application/x-www-form-urlencoded;charset=utf-8",
            "Origin": "https://gemini.google.com",
            "Referer": "https://gemini.google.com/",
            "X-Same-Domain": "1"
        }
        
        req = urllib.request.Request(url, data=data, headers=headers, method='POST')
        
        try:
            time.sleep(1.0) # Grace delay
            response = urllib.request.urlopen(req, context=self.ssl_context, timeout=30)
            if response.status == 200:
                body = response.read().decode('utf-8')
                clean_body = body.replace(")]}'", "").strip()
                
                # Simple string search as JSON parsing can be fragile with batch responses
                search_id = re.sub(r'[^a-zA-Z0-9_]', '', str(image_id))
                
                if search_id in clean_body:
                    parts = clean_body.split(search_id)
                    if len(parts) > 1:
                        segment = parts[1]
                        match = re.search(r'(https://lh3\.googleusercontent\.com/gg/[^\s"\'\\]+)', segment)
                        if match:
                            return match.group(1).replace('\\', '')
            return None
        except Exception as e:
            self.log(f"FullRes check failed: {e}")
            return None

    def parse_response(self, response_body):
        
        self.log("Parse Response...")
        
        try:
            clean_body = response_body.replace(")]}'", "").strip()
            main_array = json.loads(clean_body)
            
            if not isinstance(main_array, list):
                return None, None

            final_text = ""
            image_urls = []
            
            # 1. Parse for Text and Candidate Images
            for chunk in main_array:
                if isinstance(chunk, list) and len(chunk) >= 3 and chunk[0] == "wrb.fr":
                    inner_str = chunk[2]
                    try:
                        inner = json.loads(inner_str)

                        # Extract candidates
                        candidates = inner[4] if len(inner) > 4 else None
                        if candidates and isinstance(candidates, list):
                            for cand in candidates:
                                # Text
                                if len(cand) > 1 and isinstance(cand[1], list):
                                    text = cand[1][0]
                                    if text and "googleusercontent.com" not in text: # Avoid image placeholders
                                        final_text = text
                                
                                # Images
                                if len(cand) > 12:
                                    img_data = cand[12]
                                    # Candidate images structure...
                                    # Deep search in candidate for URL
                                    s_json = json.dumps(cand)
                                    ids = re.findall(r'c_[a-z0-9]{16}', s_json)
                                    for img_id in ids:
                                        # Try to get high res
                                        hr = self.get_full_res_url(img_id)
                                        if hr and hr not in image_urls:
                                            image_urls.append(hr)
                                            
                    except:
                        pass

            # 2. Deep Scan for any image URLs (Backup)
            if not image_urls:
                 previews = re.findall(r'https://lh3\.googleusercontent\.com/gg-dl/[^\s"\'\\]+', clean_body)
                 for url in previews:
                     clean_url = url.replace('\\', '')
                     if clean_url not in image_urls:
                         image_urls.append(clean_url)

            self.log({final_text})
            self.log(image_urls[0])

            return final_text, image_urls
            
        except Exception as e:
            self.log(f"Parse error: {str(e)}")
            return final_text, image_urls

    def download_file(self, url, save_path):
        # Enforce high res if it's a google content URL
        if 'googleusercontent.com' in url:
            base_url = re.sub(r'=[-\w]+$', '', url)
            url = f"{base_url}=s2048"

        headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
            "Cookie": self.cookie_str
        }
        
        req = urllib.request.Request(url, headers=headers)
        
        try:
            response = urllib.request.urlopen(req, context=self.ssl_context, timeout=60)
            if response.status == 200:
                with open(save_path, 'wb') as f:
                    f.write(response.read())
                return True
        except Exception as e:
            self.log(f"Download error: {str(e)}")
            return False

    # History Cleanup Methods
    def get_conversation_list(self):
        """List recent conversations via y603fc"""
        url = "https://gemini.google.com/_/BardChatUi/data/batchexecute"
        
        # [50, null, 1]
        inner_payload = [[["y603fc", "[50,null,1]", None, "generic"]]]
        f_req = json.dumps(inner_payload)
        
        data = urllib.parse.urlencode({"at": self.snlm0e, "f.req": f_req}).encode('utf-8')
        
        headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
            "Cookie": self.cookie_str,
            "Content-Type": "application/x-www-form-urlencoded;charset=utf-8",
            "X-Same-Domain": "1"
        }
        
        req = urllib.request.Request(url, data=data, headers=headers, method='POST')
        
        try:
            response = urllib.request.urlopen(req, context=self.ssl_context, timeout=30)
            if response.status == 200:
                body = response.read().decode('utf-8')
                clean = body.replace(")]}'", "").strip()
                outer = json.loads(clean)
                inner_str = outer[0][2]
                inner_data = json.loads(inner_str)
                # conversations list is inner_data[0]
                return inner_data[0]
        except Exception as e:
            self.log(f"List conv error: {e}")
        return []

    def delete_conversation(self, conv_id):
        """Delete conversation via Gz76id"""
        url = "https://gemini.google.com/_/BardChatUi/data/batchexecute"
        
        # ["c_xxxxxxxx", 1]
        inner_payload = [[["Gz76id", f"[\"{conv_id}\",1]", None, "generic"]]]
        f_req = json.dumps(inner_payload)
        
        data = urllib.parse.urlencode({"at": self.snlm0e, "f.req": f_req}).encode('utf-8')
        
        headers = {
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
            "Cookie": self.cookie_str,
            "Content-Type": "application/x-www-form-urlencoded;charset=utf-8",
            "X-Same-Domain": "1"
        }
        req = urllib.request.Request(url, data=data, headers=headers, method='POST')
        try:
            urllib.request.urlopen(req, context=self.ssl_context, timeout=10)
            return True
        except:
            return False

def rotate_cookies(psid, psidts=None):
    """
    Rotate __Secure-1PSIDTS cookie via Google's RotateCookies endpoint.
    
    Args:
        psid: Current __Secure-1PSID cookie value
        psidts: Current __Secure-1PSIDTS cookie value (optional)
        
    Returns:
        tuple: (new_psidts, success) - New PSIDTS value if successful, None otherwise
    """
    if not psid:
        return None, False
    
    url = "https://accounts.google.com/RotateCookies"
    
    # Build cookie header
    cookie_str = f"__Secure-1PSID={psid}"
    if psidts:
        cookie_str += f"; __Secure-1PSIDTS={psidts}"
    
    headers = {
        "Content-Type": "application/json",
        "Cookie": cookie_str,
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36"
    }
    
    # Request body as specified in Gemini-API
    data = b'[000,"-0000000000000000000"]'
    
    ssl_context = ssl.create_default_context()
    req = urllib.request.Request(url, data=data, headers=headers, method='POST')
    
    try:
        response = urllib.request.urlopen(req, context=ssl_context, timeout=30)
        
        # Extract new __Secure-1PSIDTS from response cookies
        cookies_header = response.headers.get('Set-Cookie', '')
        
        # Parse multiple Set-Cookie headers
        new_psidts = None
        for header in response.headers.get_all('Set-Cookie') or []:
            if '__Secure-1PSIDTS=' in header:
                # Extract the value
                match = re.search(r'__Secure-1PSIDTS=([^;]+)', header)
                if match:
                    new_psidts = match.group(1)
                    break
        
        if new_psidts:
            print(f"[CookieRotation] ✓ Got new PSIDTS: {new_psidts[:20]}...")
            return new_psidts, True
        else:
            print("[CookieRotation] Response OK but no new PSIDTS in cookies")
            return None, False
            
    except urllib.error.HTTPError as e:
        if e.code == 401:
            print("[CookieRotation] ✗ 401 Unauthorized - cookies may be expired")
        else:
            print(f"[CookieRotation] ✗ HTTP Error: {e.code}")
        return None, False
    except Exception as e:
        print(f"[CookieRotation] ✗ Error: {str(e)}")
        return None, False
