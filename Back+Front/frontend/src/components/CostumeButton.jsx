import React from 'react'
const FormButton = ({ ...props }) => {
  return (
    <div className='form_btn'>
      <button type={props.type} onSubmit={props.onSubmit} >{props.btnText}</button>
    </div>
  )
}

export default FormButton