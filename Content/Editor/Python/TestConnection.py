import unreal
import json
import os
import gemini_api

def TestConnection():

    psid, psidts = get_cookies_from_anywhere(context)

    client = gemini_api.GeminiClient(psid, psidts)
    client.log_callback = print
    token = client.get_snlm0e()
    
    if token:
        print(f"SUCCESS! Token found: {token[:10]}...")
        self.report({'INFO'}, "Success! Check System Console.")
        return {'FINISHED'}
        
    # Test 2: Just PSID (if PSIDTS was used)
    if psidts:
        print("\nTest 2: Retrying with ONLY PSID (ignoring PSIDTS)...")
        client = gemini_api.GeminiClient(psid, None)
        client.log_callback = print
        token = client.get_snlm0e()
        
        if token:
            print(f"SUCCESS (No PSIDTS)! Token: {token[:10]}...")
            self.report({'INFO'}, "Success (without PSIDTS)! Check Console.")
            return {'FINISHED'}
            
    #print("\nFAILED: Could not get token.")
    #print("Please check the debug file: 'nano_no_token.html' in your User folder.")
    #print("If that file shows a Google Login page, your cookies are invalid.")
    
    self.report({'ERROR'}, "Failed. Check System Console for details.")
    return {'FINISHED'}