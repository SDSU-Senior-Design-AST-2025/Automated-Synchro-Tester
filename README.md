# Automated-Synchro-Tester
Automated Synchro Tester Senior Design Project 

Team Workflow (LabWindows/CVI and Git) 

If you've never used Git before: don't fret! Follow this guide and you should be fine..

##1) Install Git 
 Windows: Install **Git for Windows** (this would include git bash)
 After install, open **Git Bash** and run: 
- 'git --version'

##2) Configure your name/email (do this only ONE TIME)
In git bash: 
'''bash
git config --global user.name "Your name" 
git config --global user.email "you@example.com"

##2) Install Labwindows/CVI

##3) Install Global Connect (SDSU VPN)
  You'd need VPN to use Labwindows so here is a guide on how to use VPN per SDSU: https://security.sdsu.edu/services/virtual-private-network

  
##4) In the repo webpage, click Code -> copy the HTTPS URL 
  -In git bash, go to a desired project location (example: Desktop) 
  On git it would be like: 
  cd ~/Desktop
  git clone <PASTE_REPO_URL_HERE> 

  then Enter the folder:
  cd Automated-Synchro-Tester

##5) For Daily Team Work Flow (every time you work) 
Everyone works on their own branch, and you do a Pull Request! 
-Start your work day like this: 
git checkout main
git pull 
-Then create a new branch for our work: 
git checkout -b <your-branch-name-of-choice-here> 
-Do coding changes in Labwindows/CVI along the way 

After coding a substantial amount or any amount of coding, 
please stage them and commnt them to your branch to save progress.
to Stage files: 
git add . 
git commit -m "explain what you changed" 
git push -u origin <your-branch-name-of-choice-here>

These actions above will stage the code, commit it to your branch, and it would be pushed onto Github for 
other collabs to see. 

##6) Open up a Pull Request (this will ensure nothing gets overwritten on Github) 
1. go to repo webpage
2. click on "Compare and Pull Request"
3. You should be on base branch: main
4. Compare branch: your branch
5. Fill in:
   -TItle: short summary
   -Description: what changed and how to test it
6. Create the Pull Request
7. Assign it a reviewer
8. Whoever reviews will need to approve and merge it (this is our version control!)

##7) Keep your branch up to date (this might be the hardest thing to get a hang of) 
Do this EVERY time you open/update your branch to ensure you don't push old code/version

git checkout main
git pull
git checkout <your-branch-name>
git merge main 

IF there are changes to be made, Git will tell you which files have conflicts. TO fix: 
git add .
git commit -m "resolve conflict issues"
git push

