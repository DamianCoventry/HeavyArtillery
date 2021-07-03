//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


extern size_t TextureGame;
extern size_t TextureDirt;
extern size_t TextureGrass;
extern size_t TextureMud;
extern size_t TextureRock;
extern size_t TextureSand;
extern size_t TextureSnow;
extern size_t TextureStone;


extern void CreateLoadingScreen(void);
extern void DeleteLoadingScreen(void);

extern void BeginLoadingScreen(size_t Texture, const std::string& String, size_t Min, size_t Max);
extern void EndLoadingScreen(const std::string& String, size_t Value);
extern void UpdateLoadingScreen(size_t Value);
