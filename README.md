# C-Entry gate DESN2000
## Description
Problem statement: A company has asked your team to develop an early prototype of a smart access security system for a commercial office building. The system should manage who is allowed to enter and exit the building, respond appropriately to unauthorised access attempts, and be customisable so the building management can set specific access rules. 

Assumptions: 
1) No Entry outside of business hours 
2) Uniform walking speed (2s)
3) Gate cannot be jumped over 
4) Enter and exit the gate together will not be considered 
5) No physical damage can be done to the system 

Cases: 
1) Normal Authorised Entry 
idle -> LDR1 detected -> shift register on -> read card (authorised) -> short beep + led flash + 'welcome' -> open gate -> wait 2s -> LDR2 detected -> close gate -> shift register off -> idle

2) Normal Unauthorised Entry 
idle -> LDR1 detected -> shift register on -> read card (unauthorised) -> long beep + led flash + 'invlaid card' -> shift register off -> idle

3) Tailgating 
idle -> LDR1 detected -> shift register on -> read card (authorised) -> short beep + led flash + 'welcome' -> open gate -> wait 2s -> *LDR1 detected*+ (taigate - enter lock state) -> long beep + led flash + shift register flash -> LDR2 detected x2+ -> close gate -> lock state

unlock by staff: 
lock state -> read staff card (authorised) -> enter password (keypad) -> idle state

4) Tapping twice 
idle -> LDR1 detected -> shift register on -> read card (authorised) -> short beep + led flash + 'welcome' -> open gate -> read card (authorised) -> short beep + led flash + 'Already tapped' -> wait 2s -> LDR2 detected -> close gate -> shift register off -> idle

5) Event mode 
scheduling:
(not yet planned)

on event day: 
idle state -> event mode (at specified time) -> shift register on + gate open 

after event:
event mode -> idle state (at specified time) -> shift register off + gate close

6) emergency mode 
SW1 pressed -> gate open + alarm + LED flash

unlock by staff:
emergency mode -> read staff card (authorised) -> enter password (keypad) -> idle

7) Exit 
idle -> LDR2 detected -> gate open -> wait 2s -> LDR1 detected -> idle

8) Gate forced open 
(not yet planned)

## Roadmap
setting up 
    |
Normal Authorised Entry 
    |
Other normal Entry 
    |
Testing 1 
    |
Event mode and Emergency mode 
    | 
Testing 2
    |
Extra features 

## Contributing
State if you are open to contributions and what your requirements are for accepting them.

For people who want to make changes to your project, it's helpful to have some documentation on how to get started. Perhaps there is a script that they should run or some environment variables that they need to set. Make these steps explicit. These instructions could also be useful to your future self.

You can also document commands to lint the code or run tests. These steps help to ensure high code quality and reduce the likelihood that the changes inadvertently break something. Having instructions for running tests is especially helpful if it requires external setup, such as starting a Selenium server for testing in a browser.

## Authors and acknowledgment
Agatha Komarudin 
Anusha Mukhopadhyay
Xincy Theng

## Project status
setting up

test

****
## Getting started

To make it easy for you to get started with GitLab, here's a list of recommended next steps.


## Collaborate with your team

* [Invite team members and collaborators](https://docs.gitlab.com/user/project/members/)
* [Create a new merge request](https://docs.gitlab.com/user/project/merge_requests/creating_merge_requests/)
* [Automatically close issues from merge requests](https://docs.gitlab.com/user/project/issues/managing_issues/#closing-issues-automatically)
* [Enable merge request approvals](https://docs.gitlab.com/user/project/merge_requests/approvals/)
* [Set auto-merge](https://docs.gitlab.com/user/project/merge_requests/auto_merge/)

## Test and Deploy

Use the built-in continuous integration in GitLab.

* [Get started with GitLab CI/CD](https://docs.gitlab.com/ci/quick_start/)
* [Analyze your code for known vulnerabilities with Static Application Security Testing (SAST)](https://docs.gitlab.com/user/application_security/sast/)
* [Deploy to Kubernetes, Amazon EC2, or Amazon ECS using Auto Deploy](https://docs.gitlab.com/topics/autodevops/requirements/)
* [Use pull-based deployments for improved Kubernetes management](https://docs.gitlab.com/user/clusters/agent/)
* [Set up protected environments](https://docs.gitlab.com/ci/environments/protected_environments/)
