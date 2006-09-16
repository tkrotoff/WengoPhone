# - MACRO_ADD_PLUGIN(name ${sources})
#
# Add a macro to create a plugin. A plugin is a module
# without a prefix like "lib"

MACRO (MACRO_ADD_PLUGIN _name _sources)

  ADD_LIBRARY(${_name} MODULE "${_sources}")
  SET_TARGET_PROPERTIES(${_name} PROPERTIES PREFIX "")

ENDMACRO (MACRO_ADD_PLUGIN _name _sources)

