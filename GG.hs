module Main where

import Control.Monad
import Control.Applicative
import Control.Monad.Random

import System.Random.Shuffle

import qualified Data.Vector as V

main :: IO ()
main = do
  n <- read <$> getLine
  p <- read <$> getLine :: IO Double
  gr <- V.forM [1..n] $ \i -> V.forM [1..n] $ \j -> do
    d <- getRandom
    return (d <= p && i /= j)
  perm <- fmap V.fromList $ shuffleM [0..n-1]
  
  putStrLn . show $ n
  forM_ [0..n-1] $ \i -> do
    putStr $ show (V.length . V.filter (== True) $ gr V.! i) ++ " "
    forM_ [0..n-1] $ \j -> do
      when (gr V.! i V.! j)
        $ putStr $ show j ++ " "
    putStrLn ""
  putStrLn ""
  putStrLn . show $ n
  
  forM_ [0..n-1] $ \i -> do
    putStr $ show (V.length . V.filter (== True) $ gr V.! (perm V.! i)) ++ " "
    forM_ [0..n-1] $ \j -> do
      when (gr V.! (perm V.! i) V.! (perm V.! j))
        $ putStr $ show j ++ " "
    putStrLn ""
  putStrLn ""
  forM_ [0..n-1] $ \i -> putStr $ show (perm V.! i) ++ " "
  putStrLn ""
      
