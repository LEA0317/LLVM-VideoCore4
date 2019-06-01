file(REMOVE_RECURSE
  "Attrs.inc"
  "AttrImpl.inc"
  "AttrTextNodeDump.inc"
  "AttrNodeTraverse.inc"
  "AttrVisitor.inc"
  "StmtNodes.inc"
  "DeclNodes.inc"
  "CommentNodes.inc"
  "CommentHTMLTags.inc"
  "CommentHTMLTagsProperties.inc"
  "CommentHTMLNamedCharacterReferences.inc"
  "CommentCommandInfo.inc"
  "CommentCommandList.inc"
  "StmtDataCollectors.inc"
  "CMakeFiles/ClangAttrImpl"
  "AttrImpl.inc"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ClangAttrImpl.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
