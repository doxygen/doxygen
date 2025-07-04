# Reference: https://www.tug.org/texlive/doc/install-tl.html#PROFILES

#=[ Schemes ]===================================================================
# You can pick your scheme.  Availability depends upon the package repository,
# but common ones are: infraonly, minimal, basic, small, medium and full.
selected_scheme scheme-medium

#=[ Collections ]===============================================================
# For finer control, select `scheme-custom` above and pick your collections.
# You can list them, one per line, as follows:
#   collection-<COLLECTION_NAME> 1

#=[ Paths ]=====================================================================
# These paths are the defaults in portable mode, which the action enforces.
# They are here for reference only and you should not modify them.
#   TEXDIR $TEXLIVE_INSTALL_PREFIX
#   TEXMFLOCAL $TEXLIVE_INSTALL_PREFIX/texmf-local
#   TEXMFSYSVAR $TEXLIVE_INSTALL_PREFIX/texmf-var
#   TEXMFSYSCONFIG $TEXLIVE_INSTALL_PREFIX/texmf-config
#   TEXMFVAR $TEXMFSYSVAR
#   TEXMFCONFIG $TEXMFSYSCONFIG
#   TEXMFHOME $TEXMFLOCAL

#=[ Installer options ]=========================================================
# Installer options can be provided but most have little use here.
# Note for power users: `instopt_portable` is enforced by the action
# to allow caching, so setting it here has no effect.

#=[ TeX Live Package DataBase options ]=========================================
# Only three TLPDB options are relevant.  Also, you should always set them for
# a responsible usage of storage space.
# Avoid installing package documentation files:
tlpdbopt_install_docfiles 0
# Avoid installing package source files:
tlpdbopt_install_srcfiles 0
# Avoid keeping backups:
tlpdbopt_autobackup 0

#=[ Platform options ]==========================================================
# You should not set the platform unless you know what you're doing, and let
# tlmgr and the action autodetect it.
