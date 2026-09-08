# ============================================================================
# Custom Build Configuration Overrides
# DJI-style dark UI skin for QGroundControl
# ============================================================================

# ----------------------------------------------------------------------------
# Application Branding
# ----------------------------------------------------------------------------
set(QGC_APP_NAME "QGroundControl" CACHE STRING "App Name" FORCE)

# ----------------------------------------------------------------------------
# Feature Set Customization
# ----------------------------------------------------------------------------
# Intentionally empty: this overlay is a visual skin only. It does not restrict
# firmware plugin factories or vehicle support, so a skinned build stays
# functionally identical to stock QGC.
