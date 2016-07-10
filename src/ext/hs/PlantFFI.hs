
module PlantFFI where

import Foreign
import CString
import SimEngine
import NoLoadModule as LM
import GLView
import GLHelper
import Texture
import IOExts

data Plant = Plant SimEngine Textures

type PlantRef = IORef Plant

foreign export ccall mk_plant :: CString -> IO (StablePtr PlantRef)

foreign export ccall simul :: StablePtr PlantRef -> IO ()
foreign export ccall render :: StablePtr PlantRef -> IO ()

mk_plant cs = do
    s <- peekCString cs
    putStrLn $ "load plant: " ++ s

    let (mod,model') = break (==':') s
    let model = dropWhile (==':') model'

    LM.init_fg
--    mapM_ LM.load_obj all_mods
    LM.compile_fg mod mod
    Just h <- LM.load_obj mod
    mk_sim <- LM.load_sym h (mod ++ "_mkzuplants_closure")
    ss <- mk_sim

    Just s <- return $ lookup model ss

    ts <- texInit ["leaf1","tuja", "leaf_green", "leaf_lightgreen", "leaf_brown", "leaf_red", "leaf_yellow", "leaf1faded"]
    r <- newIORef (Plant s ts)
    p <- newStablePtr r
    return p


simul :: StablePtr PlantRef -> IO ()
simul p = do
    r <- deRefStablePtr p
    Plant eng ts <- readIORef r
    writeIORef r (Plant (se_simulate_step eng) ts)
    putStrLn ("haskell simul " ++ show (se_get_size eng))


render p = do
    r <- deRefStablePtr p
    Plant eng ts <- readIORef r
    draw (0,(0,-1,0)) (Left (se_get_geom eng)) ts
    sphere 0.2 10 10
    putStrLn "haskell render"




