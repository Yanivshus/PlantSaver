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

        setLoading(true); // Set loading state
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

    const generateLi = () => {
        if (data.sentence) {
            const arr = String(data.sentence).split(" | ");
            return (
                <>
                    {arr.map((item, index) => (
                        <li key={index}>{item}</li>
                    ))}
                </>
            );
        }
        return null; // Return null if there's no sentence
    }

    return (
    <>
        <main>
            <center>
                <form onSubmit={onSubmitForm}>
                    <CostumeInput icon={<AiOutlineBulb />} type={'text'} placeholder={'Enter device name'} value={deviceName}  onChange={(e => setDeviceName(e.target.value))}/>
                    <FormButton type='submit' btnText={'Click me'} onSubmit={onSubmitForm}/>
                </form>

                <div>
                    {loading ? (
                        <p>Loading...</p>
                    ) : error ? (
                        <p>Error : {error.message} </p>
                    ) : (
                        <ul> 
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
