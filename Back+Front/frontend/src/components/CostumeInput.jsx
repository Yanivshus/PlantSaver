import React, {useState} from 'react';
const CostumeInput =  ({...props}) =>{
    return (
        <>
            <div className="input_wrapp">
                <div className='input_container'>
                    {props.icon &&
                        <div className="input_icon">
                            {props.icon}
                        </div>
                    }

                    <input
                        type={props.type}
                        placeholder={props.placeholder}
                        value={props.value}
                        onChange={props.onChange}
                        name={props.name}
                    />

                </div>
            </div>
        </>
    )
}

export default CostumeInput