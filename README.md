GObject based eXperimental Facebook Graph API library written in C.

> This library is in a very early stage of development, don't use in a final application right now. Use it just for test purposes at your own risk.  
>
>  GFBGraph maintainer

# Branch
#### Master
Master branch has the latest changes such as fully support g_autoptr, remove using of deprecated APIs and so forth, and it follows the latest version of Facebook Graph API. But it could not only break the API and ABI but also crash your system. So, don't use this branch for the development of the product unless you want to test new features.

#### v0.2-upstream
This branch is compatible with the original GFBGraph library of GNOME, which means that, **1) Not to break the API and ABI**, **2) But fix micro bugs such as memory leaks**. If you need the library and hope at least more stable than ``master``, this might be for you. But remember that as the original maintainer said, this is in a very early stage.

# Contribution
Feel free to open issues, but keep in mind that your issues could be rejected for some reasons.
