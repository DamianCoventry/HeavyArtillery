//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Wednesday, 26 June 2002
//
//========================================================================================


extern size_t CreateTexture( const std::string& FileName, size_t *Width, size_t *Height, size_t Filter, size_t RepeatTex );
extern void DeleteTexture( size_t ID );
extern void DeleteTextures( size_t Count, size_t *IDs );

extern size_t ReloadTexture( size_t ID );
extern void ReloadAllTextures( void );

extern GLuint GetTextureNumber( size_t ID );
