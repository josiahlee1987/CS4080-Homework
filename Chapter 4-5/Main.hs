-- INTERFACE
data ShapeOps = ShapeOps {
    area      :: Double,
    perim     :: Double
}


-- CONSTRUCTORS (DATA TYPES)
circle :: Double -> ShapeOps
circle r = ShapeOps {
    area      = 3.14 * r * r,
    perim     = 3.14 * 2 * r
}

square :: Double -> ShapeOps
square s = ShapeOps {
    area      = s * s,
    perim     = 4 * s
}

-- MAIN TESTING
main :: IO ()
main = do
    let myCircle = circle 2.0
    let mySquare = square 3.0

    putStrLn "--- Circle Stats (Radius: 2.0) ---"
    putStr "Area: " 
    print (area myCircle)
    putStr "Perimeter: "
    print (perim myCircle)
    
    putStrLn ""

    putStrLn "--- Square Stats (Side: 3.0) ---"
    putStr "Area: "
    print (area mySquare)
    putStr "Perimeter: "
    print (perim mySquare)