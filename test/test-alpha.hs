import Math.Gamma
import Control.Monad

alpha :: Double -> Double
alpha m = (gamma(-1.0/m) * (2 ** (-1.0/m) - 1)/(log(2))) ** (-m)

main = do
    forM_ [2,3,4,5,6,7,8] $ \n -> do
        putStrLn $ show n ++ " -> alpha = " ++ show (alpha n)
