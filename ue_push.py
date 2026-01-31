import os
import subprocess
import sys

PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))

def run_git_command(command):
    
    try:
        result = subprocess.run(
            command, 
            cwd=PROJECT_DIR, 
            check=True, 
            text=True, 
            capture_output=True
        )
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Fehler beim Befehl: {' '.join(command)}")
        print(e.stderr)
        sys.exit(1)
        
def setup_remote_if_needed():

    if not os.path.exists(os.path.join(PROJECT_DIR, ".git")):
        print("Initialisiere Git...")
        run_git_command(["git", "init"])

    try:
        subprocess.run(["git", "remote", "get-url", "origin"], 
                       cwd=PROJECT_DIR, check=True, capture_output=True)
    except subprocess.CalledProcessError:
        repo_url = input("Kein Remote gefunden. Bitte GitHub-URL eingeben: ").strip()
        if repo_url:
            run_git_command(["git", "remote", "add", "origin", repo_url])
            run_git_command(["git", "branch", "-M", "main"])

def main():
    
    setup_remote_if_needed()
    
    os.chdir(PROJECT_DIR)

    commit_msg = input("Commit-Nachricht (leer lassen für 'Quick Update'): ").strip()
    if not commit_msg:
        commit_msg = "Quick Update"

    print("--- Starte Git Sync für UE5 Projekt ---")

    print("Staging files...")
    run_git_command(["git", "add", "."])

    print(f"Committing mit Nachricht: '{commit_msg}'...")
    run_git_command(["git", "commit", "-m", commit_msg])

    print("Pushing to GitHub (LFS wird automatisch mitbehandelt)...")
    run_git_command(["git", "push", "origin", "master"])

    print("--- Erledigt! Dein UE5 Stand ist jetzt auf GitHub. ---")

if __name__ == "__main__":
    main()