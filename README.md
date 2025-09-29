# Chaos Jump

Doodle Jump clone with elastic collision and some objects that get thrown at you.
Play vs a friend and who ever gets higher wins.
When the other player dies you only have 10 seconds to get a higher score than them.

Game goes on for ever until the host decides to close the session (ESC).

Project was created in C++ using SDL3 and SteamSDK.

## Controls

Global Actions:

|Key|Action  |
|--|--|
| ESC | Go back to Main Menu  |

In-Game Actions:

|Key|Action  |
|--|--|
| A | Move left |
| D | Move right |
| S | Pull down |

Menu Actions:

|Key|Action  |
|--|--|
| H | Host Session |
| F | Open Friendslist |


Lobby Actions:

|Key|Action  |
|--|--|
| C | Close Session |
| I | Open Invite Dialogue |
| L | Start game solo (lame) |

### Good to know:

When the host leaves, also the client gets kicked into the Main Menu again.
When a client leaves, host can continue playing alone, can leave by pressing ESC.