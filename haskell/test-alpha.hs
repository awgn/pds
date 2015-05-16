import Math.Gamma
import Control.Monad

alpha :: Double -> Double
alpha m = ( gamma(-1.0/m) * (2 ** (-1.0/m) - 1)/(log(2.0))) ** (-m)

ratio :: Double -> Double
ratio m = (2 ** (1.0/m) - 1 )/log(2.0)

main = do
    forM_ [1024] $ \n -> do
        putStrLn $ show n ++ ": gamma: " ++ show(gamma(-1.0/n))  ++ " -> alpha = " ++ show (alpha n)
