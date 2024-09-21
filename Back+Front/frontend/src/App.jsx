import { useState, useEffect } from 'react'

function App() {
  
    useEffect(() =>{
        async function fetchData(params) 
        {
            console.log(import.meta.env.VITE_API_URL)
            try {
                const response = await fetch(`${import.meta.env.VITE_API_URL}entry`)
                if(!response.ok){
                    throw new Error("Network resp bad")
                }
                const result = await response.json();
                console.log(result)
                //setData(result)
            } catch (error) {
                console.error('Error fetching data:', error)
            }
        }

        fetchData();
        
    }, [])

  return (
    <>
        hello world
    </>
  )
}

export default App
