import React, { useState, useEffect } from 'react'

function App() {
    const [data, setData] = useState([]);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);

  
    useEffect(() =>{
        fetch(`${import.meta.env.VITE_API_URL}`)
        .then((response) => response.json())
        .then((data) => {
            setData(data);
            setLoading(false);
        })
        .catch((error) => {
            setError(error);
            setLoading(false);
        })
        
    }, [])

    return (
    <>
        <div>
            {loading ? (
                <p>Loading...</p>
            ) : error ? (
                <p>Error : {error.message} </p>
            ) : (
                <ul>  
                    <li>{data.sentence}</li>
                    <li>{data.score}</li>
                </ul>
            )}
        </div>
    </>
  );
}

export default App
