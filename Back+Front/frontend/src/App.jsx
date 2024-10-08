import React, { useState, useEffect } from 'react'
import './App.css'
import CostumeInput from './components/CostumeInput';
import { AiOutlineBulb} from 'react-icons/ai'
import FormButton from './components/CostumeButton';

function App() {
    const [deviceName, setDeviceName] = useState(''); // State to hold the input value
    const [data, setData] = useState({});
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState(null);

    function onSubmitForm(e) {
        e.preventDefault(); // Prevents the default form submission behavior
        setError(null);
        setLoading(false);

        setLoading(true); // Set loading state
        if(deviceName !== null && deviceName !== ''){
            fetch(`${import.meta.env.VITE_API_URL}?device_name=${deviceName}`) // Include the input value in the API call
                .then((response) => response.json())
                .then((data) => {
                    setData(data);
                    setLoading(false);
                })
                .catch((error) => {
                    
                    setError(error);
                    setLoading(false);
                });
        }
        else{
            setError("You have to specify device name");
            setLoading(true);
        }
    }

    const generateLi = () => {
        const headers = ["Temperature", "Humidity", "Water usage", "Light exposure"];
        const descriptions = data.sentence ? String(data.sentence).split(" | ") : [];
        console.log(headers, descriptions); // Verify both arrays
        return (
            <>
                {headers.map((header, index) => (
                    <li key={index}> 
                        {/* Radio Button */}
                        <input 
                            type="radio" 
                            name="tabs" 
                            id={`tab${index}`} 
                            defaultChecked={index === 0} // First tab checked by default
                        />
                        {/* Label/Header */}
                        <label 
                            htmlFor={`tab${index}`} 
                            role="tab" 
                            aria-selected="true"  
                            aria-controls={`panel${index}`} 
                            tabIndex={index}
                        >
                            {header} {/* Show the header */}
                        </label>
    
                        {/* Tab Content/Description */}
                        <div 
                            id={`tab-content${index}`} 
                            className="tab-content" 
                            role="tabpanel" 
                            aria-labelledby={`tab${index}`} 
                            aria-hidden={index !== 0} // Hide if not first tab
                        >
                            <p>{descriptions[index] || "No data available"}</p> {/* Safely access description */}
                        </div> 
                    </li>
                ))}
            </>
        );
    }
    
    
    

    return (
    <>
        <main>
            
            <center>
                <div className='wrap_head'>
                    <h>
                        PlantSaver - get plants stats and tips
                    </h>
                </div>
                <form onSubmit={onSubmitForm}>
                    <CostumeInput icon={<AiOutlineBulb />} type={'text'} placeholder={'Enter device name'} value={deviceName}  onChange={(e => setDeviceName(e.target.value))}/>
                    <br></br>
                    <br></br>
                    <button class="btn-4" type="submit" onSubmit={onSubmitForm}><span>Read More</span></button>
                    
                </form>

                <div>
                    {loading ? (
                        <p>Loading...</p>
                    ) : error ? (
                        <p>Error : {error.message} </p>
                    ) : (
                        <ul class="tabs" role="tablist"> 
                            {generateLi()}
                        </ul>
                    )}
                </div>
            </center>
            
        </main>
    </>
  );
}

export default App
